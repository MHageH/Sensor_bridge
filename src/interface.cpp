#include "interface.h"

char control_status;

int system_id;
int autopilot_id;
int companion_id;
bool time_to_exit;

Mavlink_Messages current_messages;
mavlink_set_position_target_local_ned_t current_setpoint;

float highres_flag = 1;
int lock_read_messages = 0;

// Initialisation

void interface_intialize(void){
	// initialize attributes

	control_status = 0;      // whether the autopilot is in offboard control mode
	time_to_exit   = false;  // flag to signal thread exit

	system_id    = 1; // system id
	autopilot_id = 1; // autopilot component id
	companion_id = 0; // companion computer component id

	current_messages.sysid  = system_id;
	current_messages.compid = autopilot_id;

	}
void interface_start(void){
	//send_string("Entered autopilot_start\r\n");

	//read_messages();
	/*
	while ( not current_messages.sysid ){
		if ( time_to_exit )
			return;
		// usleep(500000); // check at 2Hz
	}

	if ( not system_id ){
		system_id = current_messages.sysid;
	}

	if ( not autopilot_id ){
		autopilot_id = current_messages.compid;
	}

	// Wait for initial position ned
	while ( not ( current_messages.time_stamps.local_position_ned && current_messages.time_stamps.attitude)){
		if ( time_to_exit )
			return;
		//usleep(500000);
	}

	*/
	/*
	// copy initial position ned
	Mavlink_Messages local_data = current_messages;
	initial_position.x        = local_data.local_position_ned.x;
	initial_position.y        = local_data.local_position_ned.y;
	initial_position.z        = local_data.local_position_ned.z;
	initial_position.vx       = local_data.local_position_ned.vx;
	initial_position.vy       = local_data.local_position_ned.vy;
	initial_position.vz       = local_data.local_position_ned.vz;
	initial_position.yaw      = local_data.attitude.yaw;
	initial_position.yaw_rate = local_data.attitude.yawspeed;
	
	*/
	//autopilot_write();

	

	return;
	}

// READ
void read_messages(void){
	bool success;               // receive success flag
	bool received_all = false;  // receive only one message
	Time_Stamps this_timestamps;

	highres_flag = 1;

	// Blocking wait for new data
	while ( !received_all ){ // and !time_to_exit
		//   READ MESSAGE
		mavlink_message_t message;

		success = serial_read_message(message);

		if(success){
			switch (message.msgid){
				case MAVLINK_MSG_ID_HEARTBEAT:{
					//printf("MAVLINK_MSG_ID_HEARTBEAT received\n");
					mavlink_msg_heartbeat_decode(&message, &(current_messages.heartbeat));
					current_messages.time_stamps.heartbeat = get_time_usec();
					this_timestamps.heartbeat = current_messages.time_stamps.heartbeat;	
					break;
					}

				case MAVLINK_MSG_ID_LOCAL_POSITION_NED:{
					//printf("MAVLINK_MSG_ID_LOCAL_POSITION_NED received\n");
					mavlink_msg_local_position_ned_decode(&message, &(current_messages.local_position_ned));
					current_messages.time_stamps.local_position_ned = get_time_usec();
					this_timestamps.local_position_ned = current_messages.time_stamps.local_position_ned;
					break;
					}

				case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:{
					//printf("MAVLINK_MSG_ID_GLOBAL_POSITION_INT received\n");
					mavlink_msg_global_position_int_decode(&message, &(current_messages.global_position_int));
					current_messages.time_stamps.global_position_int = get_time_usec();
					this_timestamps.global_position_int = current_messages.time_stamps.global_position_int;
					break;
					}

				case MAVLINK_MSG_ID_POSITION_TARGET_LOCAL_NED:{
					//printf("MAVLINK_MSG_ID_POSITION_TARGET_LOCAL_NED received\n");
					mavlink_msg_position_target_local_ned_decode(&message, &(current_messages.position_target_local_ned));
					current_messages.time_stamps.position_target_local_ned = get_time_usec();
					this_timestamps.position_target_local_ned = current_messages.time_stamps.position_target_local_ned;
					break;
					}

				case MAVLINK_MSG_ID_HIGHRES_IMU: {
					//printf("MAVLINK_MSG_ID_HIGHRES_IMU received\n");
					//gpio_toggle(GPIOD, GPIO13);
					mavlink_msg_highres_imu_decode(&message, &(current_messages.highres_imu));
					current_messages.time_stamps.highres_imu = get_time_usec();
					this_timestamps.highres_imu = current_messages.time_stamps.highres_imu;
					break;
					}

				case MAVLINK_MSG_ID_ATTITUDE: {
					//printf("MAVLINK_MSG_ID_ATTITUDE received\n");	
					mavlink_msg_attitude_decode(&message, &(current_messages.attitude));
					current_messages.time_stamps.attitude = get_time_usec();
					this_timestamps.attitude = current_messages.time_stamps.attitude;
					break;
					}

				default: break;
			} // end: switch msgid
		} // end: if read message

		if (lock_read_messages == 0){
			// Check for receipt of all items
			received_all =
					this_timestamps.heartbeat                  &&
					this_timestamps.local_position_ned;		
			} else {
				received_all = this_timestamps.local_position_ned;
				if (highres_flag == 0) break;
			}

		} 
		lock_read_messages = 1;		
	return;
	}

// Write
void interface_write(void){
	// signal startup
	// prepare an initial setpoint, just stay put
	mavlink_set_position_target_local_ned_t set_point;
	set_point.type_mask = MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_VELOCITY & MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_YAW_RATE;
	set_point.coordinate_frame = MAV_FRAME_LOCAL_NED;
	set_point.vx       = 0.0;
	set_point.vy       = 0.0;
	set_point.vz       = 0.0;
	set_point.yaw_rate = 0.0;
	
	// set position target
	current_setpoint = set_point;

	interface_write_setpoint();

		return;
	}
void interface_write_setpoint(void){
	//   PACK PAYLOAD

	// pull from position target
	mavlink_set_position_target_local_ned_t set_point = current_setpoint;

	// double check some system parameters
	if ( not set_point.time_boot_ms )
		set_point.time_boot_ms = (uint32_t) (get_time_usec()/1000);
	set_point.target_system    = system_id;
	set_point.target_component = autopilot_id;

	//   ENCODE
	mavlink_message_t message;
	mavlink_msg_set_position_target_local_ned_encode(system_id, companion_id, &message, &set_point);


	//   WRITE
	interface_write_message(message);
		//gpio_toggle(GPIOD, GPIO15);	
	return;
	}
void interface_write_message(mavlink_message_t message){
	// do the write
	serial_write_message(message);

	// Done!
	return;
	}
void interface_update_setpoint(mavlink_set_position_target_local_ned_t setpoint){

	// update_setpoint
	current_setpoint = setpoint;
	}

// Offboard Control

void enable_offboard_control(void){
	// Should only send this command once
	if ( control_status == false ){
		//   TOGGLE OFF-BOARD MODE

		// Sends the command to go off-board
		int success = toggle_offboard_control( true );
		// Check the command was written
		if ( success )
			control_status = true;
	
	} // end: if not offboard_status
	}
void disable_offboard_control(void){
	// Should only send this command once
	if ( control_status == true ){
		
		// ----------------------------------------------------------------------
		//   TOGGLE OFF-BOARD MODE
		// ----------------------------------------------------------------------

		// Sends the command to stop off-board
		int success = toggle_offboard_control( false );

		// Check the command was written
		if ( success )
			control_status = false;
		
	} // end: if offboard_status
	}
int toggle_offboard_control( bool flag ){
	// Prepare command for off-board mode
	mavlink_command_long_t com = { 0 };
	com.target_system    = system_id;
	com.target_component = autopilot_id;
	com.command          = MAV_CMD_NAV_GUIDED_ENABLE;
	com.confirmation     = true;
	//com.param1           = (int32_t) flag; // flag >0.5 => start, <0.5 => stop
	com.param1 			 = 1;

	// Encode
	mavlink_message_t message;
	mavlink_msg_command_long_encode(system_id, companion_id, &message, &com);

	// Send the message
	int len = serial_write_message(message);

	// Done!
	return len;
	}

// Control
void set_position(float x, float y, float z, mavlink_set_position_target_local_ned_t &set_position){
	set_position.type_mask = MAVLINK_MSG_SET_POSITION_TARGET_LOCAL_NED_POSITION;
	set_position.coordinate_frame = MAV_FRAME_LOCAL_NED;

	set_position.x   = x; set_position.y   = y; set_position.z   = z;
	
	}
void set__(float x, float y, float z, mavlink_set_position_target_local_ned_t &final_set_point){
		set_position( x , y  , z, final_set_point);
		interface_update_setpoint(final_set_point);
		interface_write_setpoint();
		}

uint64_t get_time_usec(void){
	static struct timeval _time_stamp;
	get_time_sec(&_time_stamp, NULL);
	return _time_stamp.tv_sec*1000000 + _time_stamp.tv_usec;
	}
