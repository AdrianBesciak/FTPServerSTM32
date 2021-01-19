/*
 * usb_stick_services.c
 *
 *  Created on: 12 sty 2021
 *      Author: Adik
 */
#include "main.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "lwip.h"
#include "usb_host.h"

#include "usb_stick_services.h"
#include "ftp_server.h"

const char * padding = "                    ";

char current_directory[CURRENT_DIRECTORY_SIZE] = "/";

UART_HandleTypeDef *huart;

void init_usb_stick_services(UART_HandleTypeDef * recv_huart) {
	huart = recv_huart;
}


FRESULT get_files_in_dir(char* path, uint8_t * files_list)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

    char fileData[200];
    files_list[0] = '\0';
    int list_index = 0;

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
            	break;  /* Break on error or end of dir */

			fileData[0] = (fno.fattrib & AM_DIR) ? 'd' : '-';	//directory?
			(fno.fattrib & AM_RDO) ? sprintf(&(fileData[1]), "r--r--r--    ") : sprintf(&(fileData[1]), "rw-rw-rw-    ");	//readable/writeable

			fileData[14] = (fno.fattrib & AM_DIR) ? '2' : '1'; //number of connections to file

			char fileSize[12];	//prepare padding and filesize string
			sprintf(fileSize, "%u", fno.fsize);
			uint8_t padLen = 12 - strlen(fileSize);

			sprintf(&(fileData[15]), " 1000     1000 %*.*s%s %s\r\n\0", padLen, padLen, padding, fileSize, fno.fname);	//stick file owner, size and name

			int written = sprintf(&(files_list[list_index]), "%s", fileData);
			list_index += written;
        }
        f_closedir(&dir);
    }
    return res;
}

UBaseType_t get_file_size(const char * file) {
    FRESULT res;
	DIR dir;
    static FILINFO fno;

    UBaseType_t file_size = 0;

    res = f_opendir(&dir, current_directory);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
            	break;  /* Break on error or end of dir */
            if (strcmp(fno.fname, file) == 0) {
            	file_size = fno.fsize;
            	break;
            }
        }
        f_closedir(&dir);
    }
    return file_size;
}

