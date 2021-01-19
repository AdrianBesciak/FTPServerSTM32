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

FRESULT get_files_in_dir(char* path, uint8_t * files_list);
void read_file(const char * path, uint8_t * buffer, UBaseType_t file_size, UBaseType_t * buff_size);
UBaseType_t get_file_size(const char * file);




#endif /* USB_STICK_SERVICES_H_ */
