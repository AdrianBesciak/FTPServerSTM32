/*
 * usb_stick_services.h
 *
 *  Created on: 12 sty 2021
 *      Author: Adik
 */

#ifndef USB_STICK_SERVICES_H_
#define USB_STICK_SERVICES_H_

#define CURRENT_DIRECTORY_SIZE 50

extern char current_directory[];
SemaphoreHandle_t mutex_FS;

FRESULT get_files_in_dir(char* path, uint8_t * files_list);
UBaseType_t get_file_size(const char * file);
UBaseType_t get_number_of_files_in_dir(char* path, UBaseType_t* nof);
void delete_file(const char * path);


void init_usb_stick_services(UART_HandleTypeDef * recv_huart, SemaphoreHandle_t mutex);


#endif /* USB_STICK_SERVICES_H_ */
