
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "lis3dh.h"
#include "spi.h"


int main(int argc, char *argv[])
{
	lis3dh_data_t accel_data;

	if (init_spi(argc, argv))
	{
		printf("Error! SPI did not init\n");
		return -1;
	}
	if (lis3dh_init())
	{
		printf("Error! ls3dh did not init\n");
		return -1;
	}

	lis3dh_set_odr(LIS3DH_ODR_LP5000HZ);

	struct timespec tv1, tv2;
	int counter = 0;
	uint64_t time;
	while(1)
	{
		if(lis3dh_read_xyz(&accel_data))
			continue;

		clock_gettime(CLOCK_MONOTONIC, &tv1);
		time = (tv1.tv_sec*(uint64_t)1E6 + tv1.tv_nsec/(uint64_t)1E3) - (tv2.tv_sec*(uint64_t)1E6 + tv2.tv_nsec/(uint64_t)1E3);
		clock_gettime(CLOCK_MONOTONIC, &tv2);

		// usleep(10000);
		if (counter > 1000)
		{
			printf("X accel: %d, read frequency %f\n", accel_data.acc_x, 1E6f/(float)time);
			counter = 0;
		}
		counter++;
	}


	return 0;
}
