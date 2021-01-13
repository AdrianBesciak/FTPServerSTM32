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
const char ftp_message_current_root_directory[] = "257 \"/\" is the current dierctory\r\n";
const char ftp_message_binary_mode[] = "200 Switching to Binary mode.\r\n";
const char ftp_message_passive_mode[] = "227 Entering Passive Mode (172,16,25,125,0,23).\r\n";
const char ftp_message_service_tmp_unavailable[] = "421\r\n";
const char ftp_message_open_data_connection[] = "150 Here comes the directory listing.\r\n";
const char ftp_message_closing_successful_data_connection[] = "226 Directory send OK.\r\n";


void ftp_init_connection(struct netconn * conn, char * logbuf) {
	netconn_write(conn, ftp_message_init_connection, sizeof(ftp_message_init_connection), NETCONN_NOCOPY);

	uint8_t user_logged_in = 0;

	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

	while( user_logged_in == 0 ) {
			struct netbuf * inbuf;
			err_t recv_err;
			char * buf;
			u16_t buflen;
			recv_err = netconn_recv(conn, &inbuf);
			if (recv_err == ERR_OK) {
				if (netconn_err(conn) == ERR_OK) {
					netbuf_data(inbuf, (void**)&buf, &buflen);
					netbuf_delete(inbuf);
					/* Login section */
					if (get_request_type(buf) == AUTH_TLS) {
						HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
					}
					/* Implemented version of authorization */
					if (get_request_type(buf) == AUTH_SSL) {
						HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
						netconn_write(conn, ftp_message_ask_for_login_and_password, sizeof(ftp_message_ask_for_login_and_password), NETCONN_NOCOPY);

						struct netbuf * inbuf2;
						err_t recv_err2;
						char * buf2;
						u16_t buflen2;
						vTaskDelay(300);
						recv_err2 = netconn_recv(conn, &inbuf2);

						sprintf(logbuf, "auth_ssl recv_err: %i, netconn_err: %i\n\r", recv_err2, netconn_err(conn));
						HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));

						if (recv_err2 == ERR_OK) {
							if (netconn_err(conn) == ERR_OK) {
								netbuf_data(inbuf2, (void**)&buf2, &buflen2);
								netbuf_delete(inbuf2);
								if (get_request_type(buf2) == USER_name) {

									char name[50];
									get_user_name(buf, name);

									vTaskDelay(50);
									sprintf(logbuf, "USER name %s\n\r", name);
									HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));


									netconn_write(conn, ftp_message_ask_for_password, sizeof(ftp_message_ask_for_password), NETCONN_NOCOPY);

									vTaskDelay(50);
									struct netbuf * inbuf3;
									err_t recv_err3;
									char * buf3;
									u16_t buflen3;

									recv_err3 = netconn_recv(conn, &inbuf3);	//returns -1 err_mem

									sprintf(logbuf, "USER_pass recv_err: %i, netconn_err: %i\n\r", recv_err3, netconn_err(conn));
									HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));

									while(recv_err3 == ERR_MEM) {
										HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
										vTaskDelay(100);
									}

									if (recv_err3 == ERR_OK) {
										if (netconn_err(conn) == ERR_OK) {
											netbuf_data(inbuf3, (void**)&buf3, &buflen3);
											netbuf_delete(inbuf3);
											HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
											if (get_request_type(buf3) == USER_password) {
												HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
												HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
												char password[50];
												get_user_password(buf, password);

												vTaskDelay(50);
												sprintf(logbuf, "USER password %s\n\r", password);
												HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
												vTaskDelay(100);
/*
												char usercmp[100];
												int usercmpresult = strncmp(user_name, name, strlen(user_name));
												sprintf(usercmp, "USERcmp: %s %s cmp: %i\n\r", user_name, name, usercmpresult);
												HAL_UART_Transmit_IT(huart, usercmp, strlen(usercmp));
												vTaskDelay(100);
												char passcmp[100];
												sprintf(passcmp, "Passcmp: %s %s cmp: %i\n\r", user_password, password, strncmp(user_password, password, strlen(user_password)) == 0);
												HAL_UART_Transmit_IT(huart, passcmp, strlen(passcmp));
												vTaskDelay(100);*/
												// user authorization not used - every connection is passed as authorized
												//if ( usercmpresult == 0/* && strncmp(user_password, password, strlen(user_password)) == 0*/) {
													user_logged_in = 1;
													netconn_write(conn, ftp_message_login_successful, sizeof(ftp_message_login_successful), NETCONN_NOCOPY);
													break;
												/*} else {
													netconn_write(conn, ftp_message_login_incorrect, sizeof(ftp_message_login_incorrect), NETCONN_NOCOPY);
													continue;
												}*/
											}
										}
									}
								}
							}
						}
					}
				}
				else
					break;
			} else
				break;
			/*user credentials request */
			if (inbuf != NULL)
				netbuf_delete(inbuf);
			netconn_write(conn, ftp_message_ask_for_login_and_password, sizeof(ftp_message_ask_for_login_and_password), NETCONN_NOCOPY);
			vTaskDelay(100);
		}
}

void process_list_command(struct netconn * conn, struct netconn * data_conn) {
	/* tell that transmission has been started */
	netconn_write(conn, ftp_message_open_data_connection, sizeof(ftp_message_open_data_connection), NETCONN_NOCOPY);

	vTaskDelay(50);
	/*here should be data transmission on data port */
	static uint8_t data_buf[DATA_BUF_SIZE];

	get_files_in_dir(current_directory, data_buf);
	netconn_write(data_conn, data_buf, strlen(data_buf), NETCONN_NOCOPY);


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

	ftp_init_connection(conn, logbuf);


	while (netconn_err(conn) == ERR_OK) {
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
				case SYSTEM_TYPE:
					netconn_write(conn, ftp_message_system_type, sizeof(ftp_message_system_type), NETCONN_NOCOPY);
					break;
				case EXTRA_FEATURES:
					netconn_write(conn, ftp_message_command_not_implemented, sizeof(ftp_message_command_not_implemented), NETCONN_NOCOPY);
					break;
				case PWD:
					netconn_write(conn, ftp_message_current_root_directory, sizeof(ftp_message_current_root_directory), NETCONN_NOCOPY);
					break;
				case BINARY_MODE:
					netconn_write(conn, ftp_message_binary_mode, sizeof(ftp_message_binary_mode), NETCONN_NOCOPY);
					break;
				case PASSIVE_MODE:
					if (data_conn != NULL) {
						netconn_delete(data_conn);
					}
					if (temp_data_conn != NULL) {
						netconn_delete(temp_data_conn);
					}
					temp_data_conn = create_new_connection(FTP_DATA_PORT);
					if (temp_data_conn) {
						sprintf(logbuf, "Succesfully created temp_data_conn\r\n");
						HAL_UART_Transmit_IT(huart, logbuf, strlen(logbuf));
						netconn_write(conn, ftp_message_passive_mode, sizeof(ftp_message_passive_mode), NETCONN_NOCOPY);

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
					process_list_command(conn, data_conn);
					netconn_close(data_conn);
					netconn_close(temp_data_conn);
					vTaskDelay(100);
					netconn_delete(data_conn);
					netconn_delete(temp_data_conn);
					break;
				default:
					netconn_write(conn, ftp_message_not_recognized_operation, sizeof(ftp_message_not_recognized_operation), NETCONN_NOCOPY);
					break;
				}

				netbuf_delete(inbuf);
				vTaskDelay(300);
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
