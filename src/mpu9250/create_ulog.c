
//System Includes
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<time.h>
#include<string.h>
#include <sys/stat.h>
// #include<timespec.h>
#include <termios.h>
#include <fcntl.h>


// Our includes
#include "messages.h"
#include "create_ulog.h"


//Local Functions
static int writeMessage(void* buf, size_t size);
static uint64_t getTimeMircos();
static void writeAddLog();
static void writeFormats();
static void writeHeader();
static int createLogFile();

//Global Variables
int fd_ulog;

int initUlog()
{
	//Creates the log file
	if(createLogFile())
		return -1;

	//Writes the header to the file
	writeHeader();

	writeFormats();

	writeAddLog();

	// uint16_t data[3] = {45u, 937u, 7495u};
	// writeData(data);

	return 0;
}

void closeLogFile()
{
	close(fd_ulog);
}


// void writeData(struct ulogDataMessage data)
void writeUlogData(int16_t data[3])
{
	char buf[1024];
	uint64_t timestamp = getTimeMircos();	
	memcpy(buf + sizeof(struct ulog_message_data_header_s), &timestamp, sizeof(uint64_t));
	memcpy(buf + sizeof(struct ulog_message_data_header_s) + sizeof(uint64_t), data, sizeof(uint16_t)*3);

	size_t msg_size = sizeof(struct ulog_message_data_header_s) + sizeof(int16_t) * 3 + sizeof(uint64_t);

	uint16_t write_msg_size = (uint16_t)(msg_size - ULOG_MSG_HEADER_LEN);
	uint16_t write_msg_id = (uint16_t) 0x0000;
	buf[0] = (uint8_t)write_msg_size;
	buf[1] = (uint8_t)(write_msg_size >> 8);
	buf[2] = (uint8_t)DATA;
	buf[3] = (uint8_t)write_msg_id;
	buf[4] = (uint8_t)(write_msg_id >> 8);

	writeMessage(buf, msg_size);

}

static int createLogFile()
{

	char logger_filepath[strlen(ROOT_DIR) + 40];	

	memset(logger_filepath,0,strlen(logger_filepath));

	char n[6];
	int m=1;
	struct stat st = {0};
	
	sprintf(n,"%03d",m);
	strcpy(logger_filepath,ROOT_DIR);
	strcat(logger_filepath,"/logs/run");
	strcat(logger_filepath,n);
	
	//Find the next run number folder that isn't in use
	while(!stat(logger_filepath, &st))
	{
		m++;
		sprintf(n,"%03d",m);
		
		strcpy(logger_filepath,ROOT_DIR);
		strcat(logger_filepath,"/logs/run");
		strcat(logger_filepath,n);
	}
	
	//Make a new directory for log files
	mkdir(logger_filepath,0700);
	printf("Saving log files in: %s\n",logger_filepath);

	//Finally finish off the logger filepath 
	strcat(logger_filepath,"/acceldata.ulog");

	fd_ulog = open(logger_filepath, O_RDWR | O_TRUNC | O_CREAT);

	if (fd_ulog < 0)
	{
		printf("Error! Could not open file\n");
		return -1;
	}
	else
		return 0;
}

static void writeAddLog()
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


static void writeFormats()
{
	struct ulog_message_format_s msg = {};
	char buf[256];
	strcpy(buf, "acceldata:uint64_t timestamp;int16_t[3] accel;");
	strcpy(msg.format, buf);
	msg.msg_type = (uint8_t)FORMAT;
	size_t size = sizeof(msg) - sizeof(msg.format) + strlen(buf);
	msg.msg_size = size - ULOG_MSG_HEADER_LEN;

	writeMessage(&msg, size);

}


static void writeHeader()
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
	return write(fd_ulog, buf, size);
}
