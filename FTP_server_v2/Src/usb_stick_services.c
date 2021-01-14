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

char current_directory[50] = "/";

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

    char logdata[200];
    files_list[0] = '\0';
    int list_index = 0;

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR && NULL) {                    /* It is a directory temporarily not use*/
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
            	logdata[0] = (fno.fattrib & AM_DIR) ? 'd' : '-';
            	(fno.fattrib & AM_RDO) ? sprintf(&(logdata[1]), "r--r--r--    ") : sprintf(&(logdata[1]), "rw-rw-rw-    ");
            	logdata[14] = (fno.fattrib & AM_DIR) ? '2' : '1';
            	char fileSize[12];
            	sprintf(fileSize, "%u", fno.fsize);
            	uint8_t padLen = 12 - strlen(fileSize);
                sprintf(&(logdata[15]), " 1000     1000 %*.*s%s %s\r\n\0", padLen, padLen, padding, fileSize, fno.fname);
                HAL_UART_Transmit_IT(huart, logdata, strlen(logdata));
                int written = sprintf(&(files_list[list_index]), "%s", logdata);
                list_index += written;
                vTaskDelay(200);
            }
        }
        f_closedir(&dir);
    }
    HAL_UART_Transmit_IT(huart, files_list, strlen(files_list));
    vTaskDelay(1000);

    return res;
}
