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
	char buf[50];
	u16_t buflen;

	char logbuf[50];

	/* initial response */
	char hello_response[20];
	sprintf(hello_response, "220 (vsFTPd 3.0.3)\r\n");
	netconn_write(conn, hello_response, sizeof(hello_response), NETCONN_NOCOPY);

	recv_err = netconn_recv(conn, &inbuf);
	sprintf(logbuf, "Recv_err: %i\n\r", recv_err);
	HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
	if (recv_err == ERR_OK) {
		if (netconn_err(conn) == ERR_OK) {
			netbuf_data(inbuf, (void**)&buf, &buflen);
			sprintf(logbuf, "Otrzymano %i znakow, wiadomosc: %s\n\r", buflen, buf);
			HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			//vTaskDelay(300);
		}
	}

	/* user credentials request */
	char response2[38];
	sprintf(response2, "530 Please login with USER and PASS.\r\n");
	netconn_write(conn, response2, sizeof(response2), NETCONN_NOCOPY);

	//vTaskDelay(50);
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
			//vTaskDelay(300);
		}
	}

	/* second user credentials request the same as previously */
	//sprintf(response2, "530 Please login with USER and PASS.\r\n");
	netconn_write(conn, response2, sizeof(response2), NETCONN_NOCOPY);

	//vTaskDelay(50);
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
			//vTaskDelay(300);
		}
	}

	/* received user name, ask for password */
	char response3[34];
	sprintf(response3, "331 Please specify the password.\r\n");
	netconn_write(conn, response3, sizeof(response3), NETCONN_NOCOPY);

	vTaskDelay(50);
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

	/* send message about succesfull login */
	char response4[23];
	sprintf(response4, "230 Login successful.\r\n");
	netconn_write(conn, response4, sizeof(response4), NETCONN_NOCOPY);

	vTaskDelay(50);
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

	/* send message about system type */
/*	char response5[15];
	sprintf(response5, "215 NAME stm.\r\n");
	netconn_write(conn, response5, sizeof(response5), NETCONN_NOCOPY);

	vTaskDelay(50);
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
	}*/

	/* respond to FEAT command that i dont know it */
/*	char response6[5];
	sprintf(response6, "500\r\n");
	netconn_write(conn, response6, sizeof(response6), NETCONN_NOCOPY);

	vTaskDelay(50);
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
	}*/

	/* respond with the current directory */
	char response7[34];
	sprintf(response7, "257 \"/\" is the current dierctory\r\n");
	netconn_write(conn, response7, sizeof(response7), NETCONN_NOCOPY);

	vTaskDelay(50);
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

	/*set binary mode */
	char response8[30];
	sprintf(response8, "200 Switching to Binary mode.\r\n");
	netconn_write(conn, response8, sizeof(response8), NETCONN_NOCOPY);

	vTaskDelay(50);
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

	/* passive mode */
	char response9[48];
	sprintf(response9, "227 Entering Passive Mode (172,16,25,125,0,23).\r\n");
	netconn_write(conn, response9, sizeof(response9), NETCONN_NOCOPY);

	vTaskDelay(50);
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

	/* tell that transmission has been started */
	char response10[39];
	sprintf(response10, "150 Here comes the directory listing.\r\n");
	netconn_write(conn, response10, sizeof(response10), NETCONN_NOCOPY);

	vTaskDelay(50);
	/*here should be data transmission on data port */


	/* tell that transmission has ended */
	char response11[24];
	sprintf(response11, "226 Directory send OK.\r\n");
	netconn_write(conn, response11, sizeof(response11), NETCONN_NOCOPY);

	vTaskDelay(50);
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
