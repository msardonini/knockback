
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

#include "mpu9250.h"
#include "spi.h"


int main(int argc, char *argv[])
{


	if (init_spi(argc, argv))
	{
		printf("Error! SPI did not init\n");
		return -1;
	}
	if (mpu9250_init())
	{
		printf("Error! mpu9250 did not init\n");
		return -1;
	}

	struct timespec tv1, tv2;
	int counter = 0;
	uint64_t time;
	int16_t data[3];
	while(1)
	{

		clock_gettime(CLOCK_MONOTONIC, &tv1);
		time = (tv1.tv_sec*(uint64_t)1E6 + tv1.tv_nsec/(uint64_t)1E3) - (tv2.tv_sec*(uint64_t)1E6 + tv2.tv_nsec/(uint64_t)1E3);
		clock_gettime(CLOCK_MONOTONIC, &tv2);


		read_accel_data(data);

		if (counter >= 1000)
		{
			printf("X: %d, Y: %d, Z: %d, frq %f\n", data[0],data[1],data[2], 1E6f/(float)time);
			counter = 0;
		}
		counter++;
	}


	return 0;
}