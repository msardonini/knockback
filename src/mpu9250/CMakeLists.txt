

set(READ_MPU9250_SRC
	mpu9250.c
	spi.c
	read_mpu9250.c
	)


add_executable(read_mpu9250 ${READ_MPU9250_SRC})


