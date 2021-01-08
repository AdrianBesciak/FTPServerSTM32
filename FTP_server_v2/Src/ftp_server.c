/*
 * ftp_server.c
 *
 *  Created on: 7 sty 2021
 *      Author: Adik
 */

#include "main.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "lwip.h"
#include "usb_host.h"

#include "lwip/api.h"

#include "ftp_server.h"

UART_HandleTypeDef * huart;

//based on http server example from lab
static void ftp_server_serve(struct netconn * conn) {
	struct netbuf * inbuf;
	err_t recv_err;
	char* buf;
	u16_t buflen;

	char logbuf[100];

	char hello_response[50];
	sprintf(hello_response, "220");
	netconn_write(conn, hello_response, sizeof(hello_response), NETCONN_NOCOPY);
	//sprintf(hello_response, "230");
	//netconn_write(conn, hello_response, sizeof(hello_response), NETCONN_NOCOPY);

	recv_err = netconn_recv(conn, &inbuf);
	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	sprintf(logbuf, "Recv_err: %i\n\r", recv_err);
	HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));

	if (recv_err == ERR_OK) {
		if (netconn_err(conn) == ERR_OK) {
			netbuf_data(inbuf, (void**)&buf, &buflen);
			sprintf(logbuf, "Otrzymano %i znakow, wiadomosc: %s\n\r", buflen, buf);
			HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			vTaskDelay(300);
		}
	}
	vTaskDelay(1000);
	netconn_close(conn);
	netbuf_delete(inbuf);
}

void ftp_server_netconn_thread(void const * arguments) {
	const ftp_init_arguments * args = arguments;
	huart = args->huart;
	uint8_t logdata[50];

	struct netconn * conn, * newconn;
	err_t err, accept_err;
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);


	/* Create a new TCP connection handle*/
	conn = netconn_new(NETCONN_TCP);

	if (conn != NULL) {
		err = netconn_bind(conn, NULL, FTP_DATA_PORT);
		if (err == ERR_OK) {
			/* Put the connection into LISTEN state */
			netconn_listen(conn);


			while(1) {
				/* accept any incoming connection */
				accept_err = netconn_accept(conn, &newconn);
				if (accept_err == ERR_OK) {
					ftp_server_serve(newconn);

					netconn_delete(newconn);
				}
			}
		}
	}




	int i = 0;
	while (1) {
		i++;
		sprintf(logdata, "ftp_thread %i\n\r", i);
		HAL_UART_Transmit_IT(huart, logdata, strlen(logdata));
		vTaskDelay(500);
	}
}
