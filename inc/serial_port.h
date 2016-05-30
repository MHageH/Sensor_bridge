#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <cstdlib>
#include <stdio.h>   // Standard input/output definitions
#include <unistd.h>  // UNIX standard function definitions
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>                  /* declaration of bzero() */
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include <sys/time.h>
#include <time.h>

#include <common/mavlink.h>

int usart_recv_blocking(void);

// Initialisation
void serial_start(void);

// Read
int serial_read_message(mavlink_message_t &message);

// Write
int serial_write_message(const mavlink_message_t &message);

// Time related
int get_time_sec(struct timeval *tv, struct timezone *tz);


// Testing 
int set_interface_attribs (int fd, int speed, int parity);
void open_custom_port(void);
void send_message(char * message);
char * custom_itoa(int i, char * buffer);

#endif 
