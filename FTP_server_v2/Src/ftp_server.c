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

/* define ftp replies */
const char ftp_message_init_connection[] = "220 (vsFTPd 3.0.3)\r\n";
const char ftp_message_ask_for_login_and_password[] = "530 Please login with USER and PASS.\r\n";
const char ftp_message_ask_for_password[] = "331 Please specify the password.\r\n";
const char ftp_message_login_successful[] = "230 Login successful.\r\n";
const char ftp_message_system_type[] = "215 NAME stm.\r\n";
const char ftp_message_not_recognized_operation[] = "500\r\n";
const char ftp_message_current_root_directory[] = "257 \"/\" is the current dierctory\r\n";
const char ftp_message_binary_mode[] = "200 Switching to Binary mode.\r\n";
const char ftp_message_passive_mode[] = "227 Entering Passive Mode (172,16,25,125,0,23).\r\n";
const char ftp_message_open_data_connection[] = "150 Here comes the directory listing.\r\n";
const char ftp_message_closing_successful_data_connection[] = "226 Directory send OK.\r\n";


//based on http server example from lab
static void ftp_server_serve(struct netconn * conn) {
	struct netbuf * inbuf;
	err_t recv_err;
	char buf[50];
	u16_t buflen;

	char logbuf[50];

	/* initial response */
	netconn_write(conn, ftp_message_init_connection, sizeof(ftp_message_init_connection), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_ask_for_login_and_password, sizeof(ftp_message_ask_for_login_and_password), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_ask_for_login_and_password, sizeof(ftp_message_ask_for_login_and_password), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_ask_for_password, sizeof(ftp_message_ask_for_password), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_login_successful, sizeof(ftp_message_login_successful), NETCONN_NOCOPY);

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
/*	netconn_write(conn, ftp_message_system_type, sizeof(ftp_message_system_type), NETCONN_NOCOPY);

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
/*	netconn_write(conn, ftp_message_not_recognized_operation, sizeof(ftp_message_not_recognized_operation), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_current_root_directory, sizeof(ftp_message_current_root_directory), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_binary_mode, sizeof(ftp_message_binary_mode), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_passive_mode, sizeof(ftp_message_passive_mode), NETCONN_NOCOPY);

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
	netconn_write(conn, ftp_message_open_data_connection, sizeof(ftp_message_open_data_connection), NETCONN_NOCOPY);

	vTaskDelay(50);
	/*here should be data transmission on data port */


	/* tell that transmission has ended */
	netconn_write(conn, ftp_message_closing_successful_data_connection, sizeof(ftp_message_closing_successful_data_connection), NETCONN_NOCOPY);

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
