
//System Includes
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<time.h>
#include<string.h>
// #include<timespec.h>


#include <termios.h>

#include <fcntl.h>


// Our includes
#include "messages.h"


static int writeMessage(void* buf, size_t size);
static uint64_t getTimeMircos();


//Global Variables
int fd;

void writeData(struct ulogDataMessage)
{
	struct ulog_message_data_header_s msg = {};
	char buf[1024];
	uint64_t timestamp = getTimeMircos();	
	memcpy(buf + sizeof(struct ulog_message_data_header_s), &timestamp, sizeof(uint64_t));
	memcpy(buf + sizeof(struct ulog_message_data_header_s) + sizeof(uint64_t), data, sizeof(uint16_t)*3);

	size_t msg_size = sizeof(struct ulog_message_data_header_s) + sizeof(uint16_t) * 3 + sizeof(uint64_t);

	uint16_t write_msg_size = (uint16_t)(msg_size - ULOG_MSG_HEADER_LEN);
	uint16_t write_msg_id = (uint16_t) 0x0000;
	buf[0] = (uint8_t)write_msg_size;
	buf[1] = (uint8_t)(write_msg_size >> 8);
	buf[2] = (uint8_t)DATA;
	buf[3] = (uint8_t)write_msg_id;
	buf[4] = (uint8_t)(write_msg_id >> 8);

	writeMessage(buf, msg_size);

}


void writeAddLog()
{
	struct ulog_message_add_logged_s msg = {};

	msg.multi_id = 0x00;
	msg.msg_id = 0x0000;
	msg.msg_type = (uint8_t)ADD_LOGGED_MSG;
	strcpy(msg.message_name, "acceldata");

	size_t size = sizeof(struct ulog_message_add_logged_s) - sizeof(msg.message_name) + strlen(msg.message_name);
	msg.msg_size = size - ULOG_MSG_HEADER_LEN;

	writeMessage(&msg, size);

}


void writeFormats()
{
	struct ulog_message_format_s msg = {};
	char buf[256];
	strcpy(buf, "acceldata:uint64_t timestamp;uint16_t[3] accel;");
	strcpy(msg.format, buf);
	msg.msg_type = (uint8_t)FORMAT;
	size_t size = sizeof(msg) - sizeof(msg.format) + strlen(buf);
	msg.msg_size = size - ULOG_MSG_HEADER_LEN;

	writeMessage(&msg, size);

}


void writeHeader()
{
	struct ulog_file_header_s header = {};
	header.magic[0] = 'U';	
	header.magic[1] = 'L';
	header.magic[2] = 'o';
	header.magic[3] = 'g';
	header.magic[4] = 0x01;
	header.magic[5] = 0x12;
	header.magic[6] = 0x35;
	header.magic[7] = 0x01; //file version 1
	header.timestamp = getTimeMircos();

	writeMessage(&header, sizeof(header));

	// write the Flags message: this MUST be written right after the ulog header
	struct ulog_message_flag_bits_s flag_bits;

	memset(&flag_bits, 0, sizeof(flag_bits));
	flag_bits.msg_size = sizeof(flag_bits) - ULOG_MSG_HEADER_LEN;
	flag_bits.msg_type = (uint8_t)FLAG_BITS;

	writeMessage(&flag_bits, sizeof(flag_bits));

}

static uint64_t getTimeMircos()
{
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return tv.tv_sec*(uint64_t)1E6 + tv.tv_nsec/(uint64_t)1E3;
}

static int writeMessage(void* buf, size_t size)
{
	return write(fd, buf, size);
}

int createLogFile()
{
	fd = open("test.ulog", O_RDWR | O_TRUNC);
	if (fd < 0)
	{
		printf("Error! Could not open file\n");
		return -1;
	}
	else
		return 0;
}

void closeLogFile()
{
	close(fd);
}

int main(int argc, char * argv[])
{
	//Creates the log file
	createLogFile();

	//Writes the header to the file
	writeHeader();

	writeFormats();

	writeAddLog();

	uint16_t data[3] = {45u, 937u, 7495u};
	writeData(data);

	//Closes the log file
	closeLogFile();
	return 0;
}