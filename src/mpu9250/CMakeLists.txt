



set(READ_MPU9250_SRC
	create_ulog.c
	mpu9250.c
	spi.c
	read_mpu9250.c
	)


add_executable(read_mpu9250 ${READ_MPU9250_SRC})


target_link_libraries(read_mpu9250
	wiringPi
	)