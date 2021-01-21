/*
 * ftp_server.h
 *
 *  Created on: 7 sty 2021
 *      Author: Adik
 */

#ifndef FTP_SERVER_H_
#define FTP_SERVER_H_

#define FTP_COMMAND_PORT 21
#define FTP_DATA_PORT 23
#define DATA_BUF_SIZE 800
#define MESSAGE_BUF_SIZE 100
#define MAX_FRAME_SIZE 1300

typedef struct ftp_init_arguments {
	UART_HandleTypeDef * huart;
	FATFS * fs;
} ftp_init_arguments;

SemaphoreHandle_t mutex_FS;

void ftp_server_netconn_thread(void const * arguments);

void init_ftp_server(SemaphoreHandle_t mutex);

#endif /* FTP_SERVER_H_ */
