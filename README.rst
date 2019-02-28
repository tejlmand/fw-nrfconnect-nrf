nRF Connect SDK: fw-nrfconnect-nrf with multi image support
############################################################

To test:
********

If you have not set up west before:
::
  # Install west, see zephyr getting started guide for instructions
  mkdir project_folder && cd project_folder
  git clone -b west-test https://github.com/hakonfam/fw-nrfconnect-nrf-1.git nrf
  unset ZEPHYR_BASE # Work around for issue fixed here https://github.com/zephyrproject-rtos/west/pull/216
  west init -l nrf
  west update
  source zephyr/zephyr-env.sh
  west build --source zephyr/samples/hello_world --board nrf9160_pca10090


This will create 4 "images" in the build system,
B0, MCUBoot, SPM and app.

It should boot hello world in the end and print to UART.
