/*
 * ReaderWriter.h
 *
 *  Created on: 2014-7-1
 *      Author: manson
 */

#ifndef READERWRITER_H_
#define READERWRITER_H_

int ReadData(int sockFd, char *buf, int bufferSize);
int WriteData(int sockFd, const char *data, int size);

#endif /* READERWRITER_H_ */
