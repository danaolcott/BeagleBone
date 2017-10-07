//////////////////////////////////////////////////
/* 9/29/17
 * Dana Olcott
 * gpio_driver.h
 * Header file for gpio driver using the /dev/
 * file system.  
 */
#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_WRITE_BUFFER_SIZE		2048

void memory_init(void);
unsigned char* memory_getActiveBuffer(void);
#endif
