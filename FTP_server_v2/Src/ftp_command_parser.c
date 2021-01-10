/*
 * ftp_command_parser.c
 *
 *  Created on: 10 sty 2021
 *      Author: Adik
 */

#include "ftp_command_parser.h"

const char * auth_tls = "AUTH TLS\r\n";
const char * auth_ssl = "AUTH SSL\r\n";
const char * user = "USER";
const char * pass = "PASS";
const char * syst = "SYST\r\n";
const char * feat = "FEAT\r\n";
const char * pwd = "PWD\r\n";
const char * type = "TYPE";
const char * pasv = "PASV\r\n";
const char * list = "LIST\r\n";


ftp_request_type get_request_type(const char * request) {

}
