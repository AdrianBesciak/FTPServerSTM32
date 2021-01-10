/*
 * ftp_command_parser.h
 *
 *  Created on: 10 sty 2021
 *      Author: Adik
 */

#ifndef FTP_COMMAND_PARSER_H_
#define FTP_COMMAND_PARSER_H_

typedef enum ftp_request_type {
	AUTH_TLS = 0,
	AUTH_SSL = 1,
	USER_name = 2,
	USER_password = 3,
	SYSTEM_TYPE = 4,
	EXTRA_FEATURES = 5,
	PWD = 6,
	BINARY_MODE = 7,
	PASSIVE_MODE = 8,
	LIST = 9,
	NOT_SUPPORTED = 10
} ftp_request_type;

ftp_request_type get_request_type(const char * request);

void get_user_name(const char * request, char * buffer);
void get_user_password(const char * request, char * buffer);



#endif /* FTP_COMMAND_PARSER_H_ */
