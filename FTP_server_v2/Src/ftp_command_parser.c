/*
 * ftp_command_parser.c
 *
 *  Created on: 10 sty 2021
 *      Author: Adik
 */

#include <string.h>
#include <stdint.h>

#include "ftp_command_parser.h"

const char * auth_tls = "AUTH TLS\r\n";
const char * auth_ssl = "AUTH SSL\r\n";
const char * user = "USER";
const char * pass = "PASS";
const char * syst = "SYST\r\n";
const char * feat = "FEAT\r\n";
const char * pwd = "PWD\r\n";
const char * cwd = "CWD ";
const char * type = "TYPE";
const char * pasv = "PASV\r\n";
const char * list = "LIST\r\n";
const char * retr = "RETR ";
const char * stor = "STOR ";
const char * dele = "DELE ";


ftp_request_type get_request_type(const char * request) {
	if ( strncmp(request, auth_tls, 8) == 0)
		return AUTH_TLS;

	if (strncmp(request, auth_ssl, 8) == 0)
			return AUTH_SSL;

	if (strncmp(request, user, 4) == 0)
		return USER_name;

	if (strncmp(request, pass, 4) == 0)
		return USER_password;

	if (strncmp(request, syst, 4) == 0)
		return SYSTEM_TYPE;

	if (strncmp(request, feat, 4) == 0)
		return EXTRA_FEATURES;

	if (strncmp(request, pwd, 3) == 0)
		return PWD;

	if (strncmp(request, cwd, 3) == 0)
			return CWD;

	if (strncmp(request, type, 4) == 0) {
		if (request[5] == 'I')
			return BINARY_MODE;
		else if (request[5] == 'A')
			return ASCII_MODE;
		else
			return NOT_SUPPORTED;
	}
	if (strncmp(request, pasv, 4) == 0)
		return PASSIVE_MODE;

	if (strncmp(request, list, 4) == 0)
		return LIST;

	if (strncmp(request, retr, 4) == 0)
		return SEND_FILE;

	if (strncmp(request, stor, 4) == 0)
		return RECV_FILE;

	if (strncmp(request, dele, 4) == 0)
		return DELETE_FILE;
}

/* assumed that buffer has enough length for user_name */
void get_user_name(const char * request, char * buffer) {
	uint8_t buff_index = 0;
	for (int i = 5; i < strlen(request); i++) {
		if (request[i] == '\r')
			break;
		buffer[buff_index] = request[i];
		buff_index++;
	}
}

/*assumed that buffer has enough lenght for password */
void get_user_password(const char * request, char * buffer) {
	uint8_t buff_index = 0;
	for (int i = 5; i < strlen(request); i++) {
		if (request[i] == '\r')
			break;
		buffer[buff_index] = request[i];
		buff_index++;
	}
}

void get_new_WD(const char *request, char * buffer) {
	uint8_t buff_index = 0;
	for (int i = 4; i < strlen(request); i++) {
		if (request[i] == '\r')
			break;
		buffer[buff_index] = request[i];
		buff_index++;
	}
}

void get_filename(const char *request, char * buffer) {
	uint8_t buff_index = 0;
	for (int i = 5; i < strlen(request); i++) {
		if (request[i] == '\r')
			break;
		buffer[buff_index] = request[i];
		buff_index++;
	}
}
