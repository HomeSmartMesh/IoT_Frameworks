pscp -pw raspberry pi@ioserv:/home/pi/IoT_Frameworks/sensors_logger/rpi_app_uart_logger/ser D:\Projects\IoT_Framworks\sensors_logger\rpi_app_uart_logger
pscp -pw raspberry D:\Projects\IoT_Framworks\sensors_logger\rpi_app_uart_logger\ser pi@rfserv:/home/pi/serrun
::don't forget to give the execution writs chmod a+x ser

::also good to copy the calib data file
pscp -pw raspberry D:\Projects\IoT_Framworks\sensors_logger\rpi_app_uart_logger\calib_data.txt pi@rfserv:/home/pi/serrun
