//System Includes
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

//Package includes
#include <wiringPi.h>

//Our includes
#include "mpu9250.h"
#include "spi.h"
#include "create_ulog.h"
#include "bc118.h"


static int processIMU();
static int logData();

int isLogging = 0;

int main(int argc, char *argv[])
{
	//Initialize the wiringpi hardware
	wiringPiSetup();

	//Initialize SPI communication
	if (init_spi(argc, argv))
	{
		printf("Error! SPI did not init\n");
		return -1;
	}


	//Initialize the IMU hardware
	if (mpu9250_init())
	{
		printf("Error! mpu9250 did not init\n");
		return -1;
	}

	//Register the interrupt function to the pin its connected to
	wiringPiISR(6, INT_EDGE_RISING, (void*)&processIMU);
	
	//Initialize the bluetooth module
	if(initBC118())
	{
		printf("Error! failed to initialize BC118\n");
		return -1;
	}

	//Sleep on the while loop
	char buf1[100], buf2[100];
	while(1)
	{
		if (rcvData(buf1, buf2, 25) == 0)
		{
			if (buf2[0] == 'r')
			{
				printf("Logging data!\n");
				logData();
			}
		}
		usleep(100000);
	}

	return 0;
}



//Function to be called on the interrupt pin on the IMU
static int logData()
{
	//Initialize the logger
	if(initUlog())
	{
		printf("Error! failed to create ulog file\n");
		return -1;
	}
	isLogging = 1;

	sleep(5);
	
	isLogging = 0;
	closeLogFile();
	printf("closing log file\n");
	return 0;
}


static struct timespec tv1, tv2;
static 	int counter = 0;

static int processIMU()
{
	uint64_t time;
	int16_t data[3];


	clock_gettime(CLOCK_MONOTONIC, &tv1);
	time = (tv1.tv_sec*(uint64_t)1E6 + tv1.tv_nsec/(uint64_t)1E3) - (tv2.tv_sec*(uint64_t)1E6 + tv2.tv_nsec/(uint64_t)1E3);
	clock_gettime(CLOCK_MONOTONIC, &tv2);


	read_accel_data(data);

	if (counter >= 1000)
	{
		printf("X: %d, Y: %d, Z: %d, frq %f\n", data[0],data[1],data[2], 1E6f/(float)time);
		counter = 0;
	}

	if (isLogging)
		writeUlogData(data);

	counter++;
	return 0;

}