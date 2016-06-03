#ifndef MAVLINK_CONTROL_H_
#define MAVLINK_CONTROL_H_

#include <iostream>
#include <interface.h>
#include <mfunctions.h>

int main(int argc, char ** argv);

// Scheduler
void commands(void);
void operation (float timer);

// Function helpers
void read_messages_helper(void);
void interface_write_helper(void);

// Command line parser 
void parse_commandline(int argc, char **argv, char *&uart_name, int &baudrate);

#endif