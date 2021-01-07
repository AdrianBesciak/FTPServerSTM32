/*
 * ftp_server.h
 *
 *  Created on: 7 sty 2021
 *      Author: Adik
 */

#ifndef FTP_SERVER_H_
#define FTP_SERVER_H_

#define FTP_DATA_PORT 21

typedef struct ftp_init_arguments {
	UART_HandleTypeDef * huart;
} ftp_init_arguments;

void ftp_server_netconn_thread(void const * arguments);



#endif /* FTP_SERVER_H_ */
