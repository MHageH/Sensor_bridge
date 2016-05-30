#include "sensor_bridge.h"

mavlink_set_position_target_local_ned_t initial_position;

// Scheduler related
bool lock_ = false;

int Program_counter = 0; 

float seconds = 0;

time_t end;
time_t begin =  time(NULL);

// Parse command line
char *RS232_DEVICE = (char*)"/dev/ttyUSB0";
int baudrate = 57600;

int count = 0;

extern Mavlink_Messages current_messages;

int main(int argc, char ** argv){
	interface_intialize();

	parse_commandline(argc, argv, RS232_DEVICE, baudrate);

	serial_start();
	interface_start();

	printf("Entering while loop \n");

	while (1) {
		commands();
	}

	return 0;
	}

// Scheduler
void commands(void){
	//
		operation(3);
	}
void operation (float timer){
	read_messages();
	interface_write_helper();

	mavlink_set_position_target_local_ned_t set_point;
	mavlink_set_position_target_local_ned_t ip = initial_position;

	switch(Program_counter){
			case 0 : 

				enable_offboard_control();
				printf("Offboard control Enabled \n");
				break;
			case 1 :
					printf("Set point 1 \n");
					set__( 1 , 0, - 2.5, set_point); break;
			case 2 :
				
						set__( 5 , 0, - 5, set_point); break;
			/* OLD  ::
			case 3 :
					set__( -2 , -2.5, - 2.5, set_point); break;
			case 4 :
					set__( -2 , 2.5, - 2.5, set_point); break; 
			case 5 :
					set__( -2 , 2.5, - 1000, set_point); break; 
			*/
			default : break;
		}
			end =  time(NULL);
				//printf("Time lapse : %d \n", end - begin);
		if ((end - begin) >= timer){
				begin = time(NULL);
				printf("Operation : %d \n", Program_counter);
				Program_counter++;
			}
		// OLD :: if (Program_counter == 0 || Program_counter == 6) { Program_counter = 1;}
		if (Program_counter == 0 || Program_counter == 3) { Program_counter = 2;}

		
		mavlink_local_position_ned_t pos = current_messages.local_position_ned;
		printf("%i CURRENT POSITION XYZ = [ % .4f , % .4f , % .4f ] \n", count, pos.x, pos.y, pos.z);
		

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