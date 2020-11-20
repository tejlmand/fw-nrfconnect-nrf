#include <zephyr.h>
#include <ei_run_classifier.h>
#include <numpy.hpp>

/* Include hardcoded features array used as input for classifier. */
#include "features.h"


static int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
	memcpy(out_ptr, features + offset, length * sizeof(float));
	return 0;
}

void main(void)
{
	/* Display output of printf immediately without buffering. */
	setvbuf(stdout, NULL, _IONBF, 0);

	printk("Edge Impulse sample\n");

	if (ARRAY_SIZE(features) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
		printk("The size of your 'features' array is not correct."
		       "Expected %d items, but had %u\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE,
			ARRAY_SIZE(features));
		return;
	}

	ei_impulse_result_t result = { 0 };

	/* Periodically perform prediction and print results. */
	while (1) {
		/* The features are stored in flash, we don't want to load everything into RAM. */
		signal_t features_signal;
		features_signal.total_length = ARRAY_SIZE(features);
		features_signal.get_data = &raw_feature_get_data;

		/* Invoke the impulse. */
		EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, true);
		printk("run_classifier returned: %d\n", res);

		if (res != 0) {
			printk("run_classifier returned an error. Program execution stopped.\n");
			return;
		}

	        printk("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
		       result.timing.dsp, result.timing.classification, result.timing.anomaly);

		/* Print prediction results. */
		printk("[");
		for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
			ei_printf_float(result.classification[ix].value);
			if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
				printk(", ");
			}
		}
		if (EI_CLASSIFIER_HAS_ANOMALY) {
			printk(", ");
			ei_printf_float(result.anomaly);
		}
		printk("]\n");

		k_msleep(2000);
	}
}
