/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <errno.h>

#include <mpsl.h>
#include <mpsl_timeslot.h>

#include "multithreading_lock.h"
#include "soc_flash_nrf.h"

#define LOG_LEVEL CONFIG_SOC_FLASH_NRF_RADIO_SYNC_MPSL_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(flash_sync_mpsl);

/* Number of retries of failing timeslots with normal priority before
 * increasing the timeslot priority to high. */
#define NUM_RETRIES_NORMAL_PRIO 1

#define TIMESLOT_EXTRA_TIME_US 100

struct mpsl_context {
	struct k_sem sem;  /* Synchronization semamphore. */
	mpsl_timeslot_session_id_t session_id; /* Timeslot session ID. */
	uint32_t duration; /* Requested duration. */
	/* Argument passed to nrf_flash_sync_exe(). */
	struct flash_op_desc *op_desc;
	mpsl_timeslot_request_t timeslot_request;
	/* Return parameter for the timeslot session. */
	mpsl_timeslot_signal_return_param_t return_param;
	int status; /* Return value for nrf_flash_sync_exe(). */
	/* Indicats whether timeslot requests are done with high priority.*/
	bool high_priority;
	/* Number of retries with normal priority. */
	uint8_t retry_count;
	atomic_t timeout; /* Indicate timeout condition to the timeslot callback. */
	uint32_t begin_timestamp_us;
};

static struct mpsl_context _context;

/**
 * Get time in milliseconds since the beginning of the timeslot.
 *
 * This should only be caled inside the timelot.
 */
static uint32_t get_timeslot_time_us(void)
{
	NRF_TIMER0->TASKS_CAPTURE[0] = 1;
	return NRF_TIMER0->CC[0];
}

static void schedule_next_timeslot(void)
{
	if (!_context.high_priority) {
		if (_context.retry_count > NUM_RETRIES_NORMAL_PRIO) {
			/* Try creating timelost requests with high prio. */
			LOG_DBG("increasing priority");
			_context.high_priority = true;
		} else {
			_context.retry_count++;
		}
	}

	_context.timeslot_request.params.earliest.priority =
		_context.high_priority ?
		MPSL_TIMESLOT_PRIORITY_HIGH : MPSL_TIMESLOT_PRIORITY_NORMAL;

	int32_t ret = mpsl_timeslot_request(_context.session_id,
			&_context.timeslot_request);

	if (ret < 0) {
		LOG_ERR("mpsl_timeslot_request failed: %d", ret);
		_context.status = -EAGAIN;
		k_sem_give(&_context.sem);
	}
}

static mpsl_timeslot_signal_return_param_t *timeslot_callback(
		mpsl_timeslot_session_id_t session_id,
		uint32_t signal)
{
	__ASSERT_NO_MSG(session_id == _context.session_id);

	if (atomic_get(&_context.timeout)) {
		return NULL;
	}

	switch (signal) {
	case MPSL_TIMESLOT_SIGNAL_START:
		if (_context.op_desc->handler(_context.op_desc->context)
				== FLASH_OP_DONE) {
			_context.status = 0;
			_context.return_param.callback_action =
					MPSL_TIMESLOT_SIGNAL_ACTION_END;
		} else {
			/* Reset the priority back to normal after a successful
			 * timeslot. */
			_context.retry_count = 0;
			_context.high_priority = false;
			_context.timeslot_request.params.earliest.priority =
				MPSL_TIMESLOT_PRIORITY_NORMAL;

			_context.return_param.callback_action =
				MPSL_TIMESLOT_SIGNAL_ACTION_REQUEST;
			_context.return_param.params.request.p_next =
				&_context.timeslot_request;
		}

		break;

	case MPSL_TIMESLOT_SIGNAL_SESSION_IDLE:
		k_sem_give(&_context.sem);
		return NULL;

	case MPSL_TIMESLOT_SIGNAL_SESSION_CLOSED:
		return NULL;

	case MPSL_TIMESLOT_SIGNAL_CANCELLED:
	case MPSL_TIMESLOT_SIGNAL_BLOCKED:
		schedule_next_timeslot();
		return NULL;

	default:
		__ASSERT(false, "unexpected signal: %u", signal);
		return NULL;
	}

	return &_context.return_param;
}

int nrf_flash_sync_init(void)
{
	LOG_DBG("");
	return k_sem_init(&_context.sem, 0, 1);
}

void nrf_flash_sync_set_context(uint32_t duration)
{
	LOG_DBG("duration: %u", duration);
	_context.duration = duration;
}

bool nrf_flash_sync_is_required(void)
{
	return mpsl_is_initialized();
}

int nrf_flash_sync_exe(struct flash_op_desc *op_desc)
{
	LOG_DBG("");

	int errcode = MULTITHREADING_LOCK_ACQUIRE();
	__ASSERT_NO_MSG(errcode == 0);
	uint32_t ret = mpsl_timeslot_session_open(timeslot_callback,
			&_context.session_id);
	MULTITHREADING_LOCK_RELEASE();

	if (ret < 0) {
		LOG_ERR("mpsl_timeslot_session_open failed: %d", ret);
		return -ENOMEM;
	}

	mpsl_timeslot_request_t *req = &_context.timeslot_request;
	req->request_type = MPSL_TIMESLOT_REQ_TYPE_EARLIEST;
	req->params.earliest.hfclk = MPSL_TIMESLOT_HFCLK_CFG_NO_GUARANTEE;
	req->params.earliest.priority = MPSL_TIMESLOT_PRIORITY_NORMAL;
	req->params.earliest.length_us =
		_context.duration + TIMESLOT_EXTRA_TIME_US;
	req->params.earliest.timeout_us = MPSL_TIMESLOT_EARLIEST_TIMEOUT_MAX_US;

	_context.op_desc = op_desc;
	_context.status = -ETIMEDOUT;
	_context.high_priority = false;
	_context.retry_count = 0;
	atomic_clear(&_context.timeout);

	__ASSERT_NO_MSG(k_sem_count_get(&_context.sem) == 0);

	errcode = MULTITHREADING_LOCK_ACQUIRE();
	__ASSERT_NO_MSG(errcode == 0);
	ret = mpsl_timeslot_request(_context.session_id, req);
	MULTITHREADING_LOCK_RELEASE();

	if (ret < 0) {
		LOG_ERR("mpsl_timeslot_request failed: %d", ret);
		mpsl_timeslot_session_close(_context.session_id);
		return -EINVAL;
	}

	if (k_sem_take(&_context.sem, K_MSEC(FLASH_TIMEOUT_MS)) < 0) {
		LOG_ERR("timeout");
		atomic_set(&_context.timeout, 1);
	}

	/* This will cancel the timeslot if it is still in progress. */
	errcode = MULTITHREADING_LOCK_ACQUIRE();
	__ASSERT_NO_MSG(errcode == 0);
	mpsl_timeslot_session_close(_context.session_id);
	MULTITHREADING_LOCK_RELEASE();

	/* Reset the semaphore after timeout, in case if the operation _did_
	 * complete before closing the session. */
	if (atomic_get(&_context.timeout)) {
		k_sem_reset(&_context.sem);
	}

	return _context.status;
}

void nrf_flash_sync_get_timestamp_begin(void)
{
	_context.begin_timestamp_us = get_timeslot_time_us();
}

bool nrf_flash_sync_check_time_limit(uint32_t iteration)
{
	uint32_t now_us = get_timeslot_time_us();
	uint32_t time_per_iteration_us =
		(now_us - _context.begin_timestamp_us) / iteration;
	return now_us + time_per_iteration_us >= _context.duration;
}
