#include "serial_port.h"

mavlink_status_t status;
uint8_t          msgReceived = false;

extern volatile float seconds;

//extern struct termios oldtio,newtio;
struct termios oldtio,newtio;
static int fd;

extern char * RS232_DEVICE;
extern int baudrate;

// Initialisation
void serial_start(void){
    printf("Open port : %s \n", RS232_DEVICE);
    fd = open(RS232_DEVICE, O_RDWR | O_NOCTTY ); 
    
        if (fd <0) {
            perror(RS232_DEVICE); 
            exit(-1); 
        }

    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
    newtio.c_cflag = baudrate | CS8 | CLOCAL | CREAD;  /* _no_ CRTSCTS */
    newtio.c_iflag = IGNPAR; // | ICRNL |IXON; 
    newtio.c_oflag = IGNPAR; //ONOCR|ONLRET|OLCUC;
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    printf("Succeeded\n");
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
  }

// Serial Read
int serial_read_message(mavlink_message_t &message){

    msgReceived = mavlink_parse_char(MAVLINK_COMM_1, usart_recv_blocking(), &message, &status);
	return msgReceived;
	}
int usart_recv_blocking(void){ 
    char c;
    read(fd,&c,1);
    //printf("Received character : %c\n", c);
    return(c);
  }

// Serial write
int serial_write_message(const mavlink_message_t &message){
    char buff[300];
    int bytesWritten;
    unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buff, &message);

    for (int i = 0; i < sizeof(buff); i++) {
     write(fd,&buff[i],1);
    }

    len = 0;
    bytesWritten = len;
    return bytesWritten;
  }

// gettimeofday
int get_time_sec(struct timeval *tv, struct timezone *tz){
  //
  gettimeofday(tv, tz);
  }

// Testing

#define error_message printf
int file_descriptor;

int set_interface_attribs (int fd, int speed, int parity){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0) {
                error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
        }
void open_custom_port(void){
    char *portname = (char *)"/dev/ttyUSB0";
    file_descriptor = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    
        if (fd < 0) {
            error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
            return;
        }   
    set_interface_attribs(file_descriptor, 57600, 0);   
    printf("Opened Serial port : %s\n", portname);
    }

void send_message(char * message){

    for (int i = 0; i < sizeof(message); i++){
        write(file_descriptor, &message[i], 1);
    }
    }

char* custom_itoa(int i, char b[]){
    
    char const digit[] = "0123456789";
    
    char* p = b;
    
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    
    int shifter = i;
    
    do{ 
        ++p;
        shifter = shifter/10;
    } while(shifter);
    
    *p = '\0';
        
    do{
        *--p = digit[i%10];
        i = i/10;
    } while(i);
        
        return b;
    
    }