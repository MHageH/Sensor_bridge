#include "sensor_bridge.h"
#include <math.h>

mavlink_set_position_target_local_ned_t initial_position;

// Scheduler related
bool lock_ = false;

int Program_counter = 0; 

float seconds = 0;

time_t end;
time_t begin =  time(NULL);

// Parse command line
char *RS232_DEVICE = (char*)"/dev/ttyUSB1";
int baudrate = 57600;

// Received coordinates count
int count = 0;

extern float m_x_position;
extern float m_y_position;
extern int position_lock;
extern float sep;
extern float Amplitude;
extern int Phi;

// Fixed Sensor position
extern float sensor_position_x;
extern float sensor_position_y;

extern Mavlink_Messages current_messages;

int main(int argc, char ** argv){
	interface_intialize();

	parse_commandline(argc, argv, RS232_DEVICE, baudrate);

	serial_start();
	interface_start();

	// Designed to send orders to STM32F4
	open_custom_port();
	//

	printf("Entering while loop \n");

	while (1) {
		commands();
	}

	return 0;
	}

// Scheduler
void commands(void){
	//
		operation(10);
	}
void operation (float timer){
	read_messages();
	interface_write_helper();

	mavlink_set_position_target_local_ned_t set_point;
	mavlink_set_position_target_local_ned_t ip = initial_position;

	mavlink_local_position_ned_t pos = current_messages.local_position_ned;

	switch(Program_counter){
			case 0 : 

				enable_offboard_control();
				printf("Offboard control Enabled \n");
				Program_counter = 1;
				break;
			case 1 :
					printf("Set point 1 \n");
					set__( 1 , 0, - 2.5, set_point); break;
			case 2 :
					set__( 5 , 0, - 5  , set_point); break;
			case 3 :
				if(amplitude(pos.x, pos.y)*1000 >= Amplitude*1000){
					mavlink_local_position_ned_t local_position = current_messages.local_position_ned;
					set__(local_position.x, local_position.y, -5, set_point); break;
					sep = 0;
				} else {
					if (position_lock == 0){
						m_x_position = pos.x;
						//printf("m_x_position set to : %f\n", m_x_position);
						m_y_position = pos.y;
						//printf("m_y_position set to : %f\n", m_y_position);
						sep = m_x_position;
						//printf("SEP set to : %f\n", sep);
						
						Phi = phase(m_x_position, m_y_position);
						//printf("Phase locked to : %d\n", Phi);
						position_lock = 1;
					}
					// OLD :: set__(sep, m_y_position - m_x_position*tanf(Phi*(M_PI/180)) + sep* tanf(Phi*(M_PI/180)), -5, set_point);
					set__(sep, m_y_position - m_x_position*tangent_m_f(Phi) + sep* tangent_m_f(Phi), -5, set_point);
					sep++;
					break;
				}
			default : break;
		}
			end =  time(NULL);
			if ((end - begin) >= timer){
				begin = time(NULL);
				printf("Operation : %d \n", Program_counter);
				Program_counter++;
			if (Program_counter == 0 || Program_counter == 4) { Program_counter = 3;}
			}
		
		//printf("%i CURRENT POSITION XYZ = [ % .4f , % .4f , % .4f ] \n", count, pos.x, pos.y, pos.z);
		
		// Just to test the ability of STM32F4 to receive the correct position on USART3 (must be changed later to USART1
		printf("\n Phase at M = %d\n", Phi);
		// OLD :: printf("Y0 at M = %f\n\n", m_y_position - m_x_position*tanf(Phi*(M_PI/180)) + 5* tanf(Phi*(M_PI/180)));
		printf("Y0 at M = %f\n\n", m_y_position - m_x_position*tangent_m_f(Phi) + 5* tangent_m_f(Phi));
		
		// printf("Phase = %d\n", phase(pos.x, pos.y));
		printf("Amplitude*1000 = %f\n", 1000*amplitude(pos.x, pos.y));
		
		count++;
	}



// Function Helpers
void interface_write_helper(void){
	if (lock_ == false) {
			interface_write();		
	}
	lock_ = true;
	}

//   Parse Command Line
void parse_commandline(int argc, char **argv, char *&uart_name, int &baudrate){

	// string for command line usage
	const char *commandline_usage = "usage: mavlink_serial -d <devicename> -b <baudrate>";

	// Read input arguments
	for (int i = 1; i < argc; i++) { // argv[0] is "mavlink"

		// Help
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("%s\n",commandline_usage);
			throw EXIT_FAILURE;
		}

		// UART device ID
		if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--device") == 0) {
			if (argc > i + 1) {
				uart_name = argv[i + 1];

			} else {
				printf("%s\n",commandline_usage);
				throw EXIT_FAILURE;
			}
		}

		// Baud rate
		if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--baud") == 0) {
			if (argc > i + 1) {
				baudrate = atoi(argv[i + 1]);

			} else {
				printf("%s\n",commandline_usage);
				throw EXIT_FAILURE;
			}
		}

	}
	// end: for each input argument

	// Done!
	return;
	}

