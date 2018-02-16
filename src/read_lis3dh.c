
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

	init_spi(argc, argv);

	lis3dh_init();

	while(1)
	{
		lis3dh_read_xyz(&accel_data);
		usleep(10000);
	}


	return 0;
}
