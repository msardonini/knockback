
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
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

	while(1)
	{
		lis3dh_read_xyz(&accel_data);

		// printf("X accel: %u\n", accel_data.acc_x);
		usleep(10000);
	}


	return 0;
}
