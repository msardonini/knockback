#ifndef BC118_H
#define BC118_H



int initBC118();

int sndData(char* command, size_t len);

int rcvData(char* buf, char* rcv_cmd, size_t len);

#endif