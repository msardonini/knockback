#ifndef CREATE_ULOG_H
#define CREATE_ULOG_H


int initUlog();

void closeLogFile();

void writeUlogData(int16_t data[3]);


#endif