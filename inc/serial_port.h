#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <cstdlib>
#include <stdio.h>   // Standard input/output definitions
#include <unistd.h>  // UNIX standard function definitions

#include <common/mavlink.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>                  /* declaration of bzero() */
#include <fcntl.h>
#include <termios.h>

#include <sys/time.h>
// MOD
#include <time.h>
//

#define USART3 3

int usart_recv_blocking(int i);

// Initialisation
void serial_start(void);

// Read
int serial_read_message(mavlink_message_t &message);

// Write
int serial_write_message(const mavlink_message_t &message);

// Time related
int get_time_sec(struct timeval *tv, struct timezone *tz);


#endif 