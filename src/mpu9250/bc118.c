
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/types.h>


#include "bc118.h"

int BC_fd;

int initBC118()
{
	struct termios newtio;
    BC_fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NONBLOCK );
    if (BC_fd < 0) { perror("/dev/serial0"); exit(-1); }

    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
    newtio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;

    tcflush(BC_fd, TCIFLUSH);
    if(tcsetattr(BC_fd,TCSANOW,&newtio))
    	printf("Fauled to set attributes for port\n");

    return 0;
}

int sndData(char* command, size_t len)
{
	char buf[64];
	strcpy(buf, "SND ");
	strcat(buf, command);
	strcat(buf, "\r");

	return write(BC_fd, buf, len+5);

}


int rcvData(char* msg, char* rcv_cmd, size_t len)
{
	/********** temp debug code *************/
	// sndData("HELLO", 5);


	size_t rcv_len;
	rcv_len = read(BC_fd, msg, len);

 	// if(rcv_len == -1)
 	// {
		// printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
 	// }

	if (rcv_len == -1 || rcv_len == 0)
		return -1;

	printf("received string %s, ret value is %d\n\n", msg, rcv_len);

	//Check the first 5 bytes for the sync byte
	int i;
	for (i = 0; i < 5; i++)
	{
		if (msg[i] == 'R')
			break;

		//Check if we never found the sync byte
		if (i == 4)
			return -1;
	}

	printf ("222i is %d, msg[i] is %c ", i, msg[i+4]);

	if (msg[i+1] == 'C' && msg[i+2] == 'V' && msg[i+3] == '=')
	{
		rcv_cmd[0] = msg[i+4];
		printf ("i is %d, msg[i] is %c ", i, msg[i]);
		// memcpy(rcv_cmd, &msg[i+4], 1);
	}

	return 0;
}





















