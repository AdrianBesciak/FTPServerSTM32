/*
 * usb_stick_services.h
 *
 *  Created on: 12 sty 2021
 *      Author: Adik
 */

#ifndef USB_STICK_SERVICES_H_
#define USB_STICK_SERVICES_H_

extern char current_directory[];

FRESULT get_files_in_dir(char* path, uint8_t * files_list);



#endif /* USB_STICK_SERVICES_H_ */
