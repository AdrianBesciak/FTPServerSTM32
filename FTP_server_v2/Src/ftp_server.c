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
#include "ftp_command_parser.h"
#include "usb_stick_services.h"

UART_HandleTypeDef * huart;

const char user_name[4] = "user";
const char user_password[4] = "pass";

/* define ftp replies */
const char ftp_message_init_connection[] = "220 (vsFTPd 3.0.3)\r\n";
const char ftp_message_ask_for_login_and_password[] = "530 Please login with USER and PASS.\r\n";
const char ftp_message_ask_for_password[] = "331 Please specify the password.\r\n";
const char ftp_message_login_successful[] = "230 Login successful.\r\n";
const char ftp_message_login_incorrect[] = "530 Login incorrect.\r\n";
const char ftp_message_system_type[] = "215 UNIX Type: L8.\r\n";
const char ftp_message_not_recognized_operation[] = "500\r\n";
const char ftp_message_command_not_implemented[] = "502 Command not implemented\r\n";
const char ftp_message_current_directory_left[] = "257 ";
const char ftp_message_current_directory_right[] = " is the current dierctory\r\n";
const char ftp_message_binary_mode[] = "200 Switching to Binary mode.\r\n";
const char ftp_message_ascii_mode[] = "200 Switching to ASCII mode.\r\n";
const char ftp_message_passive_mode[] = "227 Entering Passive Mode (172,16,25,125,0,";
const char ftp_message_service_tmp_unavailable[] = "421\r\n";
const char ftp_message_open_data_connection[] = "150 Data transmission has been started.\r\n";
const char ftp_message_closing_successful_data_connection[] = "226 Transfer complete.\r\n";
const char ftp_message_directory_changed[] = "250 Directory succesfully changed.\r\n";
const char ftp_message_request_passive_mode[] = "425 Use PASV first.\r\n";

uint8_t get_data_port() {
	static uint8_t port = FTP_DATA_PORT;
	if ( port == 0 )
		port = FTP_DATA_PORT;
	return port++;
}

void process_list_command(struct netconn * conn, struct netconn * data_conn) {
	/* tell that transmission has been started */
	netconn_write(conn, ftp_message_open_data_connection, sizeof(ftp_message_open_data_connection), NETCONN_NOCOPY);

	vTaskDelay(50);
	static uint8_t data_buf[DATA_BUF_SIZE];

	get_files_in_dir(current_directory, data_buf);
	netconn_write(data_conn, data_buf, strlen(data_buf), NETCONN_NOCOPY);


	/* tell that transmission has ended */
	netconn_write(conn, ftp_message_closing_successful_data_connection, sizeof(ftp_message_closing_successful_data_connection), NETCONN_NOCOPY);

	return;
}

void send_file(struct netconn * conn, struct netconn * data_conn, const char * file) {
	/* tell that transmission has been started */
	netconn_write(conn, ftp_message_open_data_connection, sizeof(ftp_message_open_data_connection), NETCONN_NOCOPY);

	vTaskDelay(50);
	/*here should be data transmission on data port */

	static uint8_t buffer[MAX_FRAME_SIZE];
	UBaseType_t file_size = get_file_size(file);
	UBaseType_t read_size;
	read_file(file, buffer, file_size, &read_size);
	HAL_UART_Transmit_IT(huart, buffer, read_size);
	netconn_write(data_conn, buffer, read_size, NETCONN_NOCOPY);

	/* tell that transmission has ended */
	netconn_write(conn, ftp_message_closing_successful_data_connection, sizeof(ftp_message_closing_successful_data_connection), NETCONN_NOCOPY);


	return;
}

struct netconn * create_new_connection(uint16_t port) {
	struct netconn * conn, * newconn;
	err_t err, accept_err;

	/* Create a new TCP connection handle*/
	conn = netconn_new(NETCONN_TCP);

	if (conn != NULL) {
		err = netconn_bind(conn, NULL, port);
		if (err == ERR_OK) {
			return conn;
		} else {
			return NULL;
		}
	}
	return NULL;
}

//based on http server example from lab
static void ftp_server_serve(struct netconn * conn) {
	char logbuf[50];
	struct netconn * data_conn = NULL;
	struct netconn * temp_data_conn = NULL;

	char message_buf[MESSAGE_BUF_SIZE];

	char name[50];
	char password[50];
	char filename[50];

	netconn_write(conn, ftp_message_init_connection, sizeof(ftp_message_init_connection), NETCONN_NOCOPY);

	uint8_t transmission_not_finished = 1;

	while (netconn_err(conn) == ERR_OK && transmission_not_finished) {
		struct netbuf * inbuf;
		err_t recv_err;
		char * buf;
		u16_t buflen;

		recv_err = netconn_recv(conn, &inbuf);
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
		sprintf(logbuf, "Recv_err: %i\n\r", recv_err);
		HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));

		if (recv_err == ERR_OK) {
			if (netconn_err(conn) == ERR_OK) {
				netbuf_data(inbuf, (void**)&buf, &buflen);
				/*sprintf(logbuf, "Otrzymano %i znakow, wiadomosc: %s\n\r", buflen, buf);
				HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);*/

				switch(get_request_type(buf)) {
				case AUTH_TLS:
					netconn_write(conn, ftp_message_ask_for_login_and_password, sizeof(ftp_message_ask_for_login_and_password), NETCONN_NOCOPY);
					break;
				case AUTH_SSL:
					netconn_write(conn, ftp_message_ask_for_login_and_password, sizeof(ftp_message_ask_for_login_and_password), NETCONN_NOCOPY);
					break;
				case USER_name:
					get_user_name(buf, name);

					vTaskDelay(50);
					sprintf(logbuf, "USER name %s\n\r", name);
					HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));

					netconn_write(conn, ftp_message_ask_for_password, sizeof(ftp_message_ask_for_password), NETCONN_NOCOPY);
					break;
				case USER_password:
					HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
					get_user_password(buf, password);

					vTaskDelay(50);
					sprintf(logbuf, "USER password %s\n\r", password);
					HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));

					netconn_write(conn, ftp_message_login_successful, sizeof(ftp_message_login_successful), NETCONN_NOCOPY);
					break;

				case SYSTEM_TYPE:
					netconn_write(conn, ftp_message_system_type, sizeof(ftp_message_system_type), NETCONN_NOCOPY);
					break;
				case EXTRA_FEATURES:
					netconn_write(conn, ftp_message_command_not_implemented, sizeof(ftp_message_command_not_implemented), NETCONN_NOCOPY);
					break;
				case PWD:
					sprintf(message_buf, "%s%s%s", ftp_message_current_directory_left, current_directory, ftp_message_current_directory_right);
					netconn_write(conn, message_buf, strlen(message_buf), NETCONN_NOCOPY);
					break;
				case CWD:
					get_new_WD(buf, current_directory);
					netconn_write(conn, ftp_message_directory_changed, sizeof(ftp_message_directory_changed), NETCONN_NOCOPY);
					break;
				case BINARY_MODE:
					netconn_write(conn, ftp_message_binary_mode, sizeof(ftp_message_binary_mode), NETCONN_NOCOPY);
					break;
				case ASCII_MODE:
					// TODO
					//not implemented yet,  and not important as long as with this serwer communicates only with windows machines
					netconn_write(conn, ftp_message_ascii_mode, sizeof(ftp_message_ascii_mode), NETCONN_NOCOPY);
					break;
				case PASSIVE_MODE:
					if (data_conn != NULL) {
						netconn_delete(data_conn);
					}
					if (temp_data_conn != NULL) {
						netconn_delete(temp_data_conn);
					}
					//temp_data_conn = create_new_connection(FTP_DATA_PORT);
					uint8_t port = get_data_port();
					temp_data_conn = create_new_connection(port);
					if (temp_data_conn) {
						sprintf(logbuf, "Succesfully created temp_data_conn\r\n");
						HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
						sprintf(message_buf, "%s%i).\r\n", ftp_message_passive_mode, port);
						netconn_write(conn, message_buf, sizeof(message_buf), NETCONN_NOCOPY);

						netconn_listen(temp_data_conn);
						/* accept an incoming connection */
						err_t accept_err = netconn_accept(temp_data_conn, &data_conn);
						if (accept_err == ERR_OK) {
							sprintf(logbuf, "Succesfully openned data_conn\r\n");
							vTaskDelay(100);
							HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
							break;
						}
					}
					sprintf(logbuf, "Error in creating or openning data_conn\r\n");
					HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
					netconn_write(conn, ftp_message_service_tmp_unavailable, sizeof(ftp_message_service_tmp_unavailable), NETCONN_NOCOPY);
					break;
				case LIST:
					if (data_conn == NULL) {
						netconn_write(conn, ftp_message_request_passive_mode, sizeof(ftp_message_request_passive_mode), NETCONN_NOCOPY);
						break;
					}
					process_list_command(conn, data_conn);
					netconn_close(data_conn);
					netconn_close(temp_data_conn);
					vTaskDelay(100);
					netconn_delete(data_conn);
					netconn_delete(temp_data_conn);
					//data_conn = temp_data_conn = NULL;
					transmission_not_finished = 0;
					break;
				case SEND_FILE:
					if (data_conn == NULL) {
						netconn_write(conn, ftp_message_request_passive_mode, sizeof(ftp_message_request_passive_mode), NETCONN_NOCOPY);
						break;
					}
					get_filename(buf, filename);
					send_file(conn, data_conn, filename);
					netconn_close(data_conn);
					netconn_close(temp_data_conn);
					vTaskDelay(100);
					netconn_delete(data_conn);
					netconn_delete(temp_data_conn);
					//data_conn = temp_data_conn = NULL;
					transmission_not_finished = 0;
					break;
				default:
					netconn_write(conn, ftp_message_not_recognized_operation, sizeof(ftp_message_not_recognized_operation), NETCONN_NOCOPY);
					break;
				}

				netbuf_delete(inbuf);
				vTaskDelay(300);
			} else {
				break;
			}
		}
	}

	vTaskDelay(1000);
	netconn_close(conn);
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
		err = netconn_bind(conn, NULL, FTP_COMMAND_PORT);
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
