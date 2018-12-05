/*
__/\\\\\\\\\\\\\_____/\\\\\\\\\\\__/\\\\\\\\\\\\____        
 _\/\\\/////////\\\__\/////\\\///__\/\\\////////\\\__       
  _\/\\\_______\/\\\______\/\\\_____\/\\\______\//\\\_      
   _\/\\\\\\\\\\\\\/_______\/\\\_____\/\\\_______\/\\\_     
    _\/\\\/////////_________\/\\\_____\/\\\_______\/\\\_    
     _\/\\\__________________\/\\\_____\/\\\_______\/\\\_   
      _\/\\\___________/\\\___\/\\\_____\/\\\_______/\\\__  
       _\/\\\__________\//\\\\\\\\\______\/\\\\\\\\\\\\/___
        _\///____________\/////////_______\////////////_____
-> Name:  TrainController.cpp
-> Date: Nov 29, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainController.h"

// Singleton Instance
TrainController *TrainController::TrainControllerInstance_ = 0;

TrainController::TrainController() {
    switch_msg_buffer_ = new RingBuffer<switch_ctrl_t>(SWITCH_BUFFER_SIZE);
    // train_msg_buffer_ = new RingBuffer<train_ctrl_t>(TRAIN_BUFFER_SIZE);

    for(uint8_t i = 0; i < NUM_TRAINS; i++) {
        trains_[i].current_zone               = NO_ZONE;
        trains_[i].current_dst                = NO_ZONE;
        trains_[i].default_speed              = 5; // TODO: Un-hardcode this speed

        // Set up control block
        trains_[i].train_ctrl.speed = 0;
        trains_[i].train_ctrl.num = i;
        trains_[i].train_ctrl.dir = STAY;

        trains_[i].initialized = false;
        trains_[i].stop_req = false;

        // Clear triggered_sensors
        for (uint8_t x = 0; x < MAX_DIFF_SENSORS; x++) {
            trains_[i].triggered_sensors[x].reset = 0; // Reset both fields to 0
        }
    }
}

TrainController::~TrainController() {
    delete switch_msg_buffer_;
    // delete train_msg_buffer_;
}

void TrainController::SetSwitch(switch_ctrl_t switch_ctrl) {
    switch_msg_buffer_->Add(switch_ctrl);
    TrainCommandCenterInstance_->SendSwitchCommand(switch_ctrl.num, (switch_direction_t)switch_ctrl.req_state, TRAIN_CONTROLLER_MB);
}

void TrainController::CmdTrain(train_ctrl_t train_ctrl) {
    last_train_cmds[train_ctrl.num] = train_ctrl;
    // train_msg_buffer_->Add(train_ctrl);
    TrainCommandCenterInstance_->SendTrainCommand(train_ctrl.num + 1, train_ctrl.speed, (train_direction_t)train_ctrl.dir, TRAIN_CONTROLLER_MB);
}

void TrainController::StopTrain(uint8_t train_num, bool physically_stop) {
    trains_[train_num].train_ctrl.speed = 0;
    trains_[train_num].train_ctrl.dir = STAY;
    trains_[train_num].stop_req = false;

    // Reset zone 
    trains_[train_num].current_dst = NO_ZONE;

    if (physically_stop) CmdTrain(trains_[train_num].train_ctrl);

    // Update monitor with new destination (NO_ZONE)
    static uint8_t msg_body[3];
    msg_body[0] = TRAIN_GO_CMD;
    msg_body[1] = train_num;
    msg_body[2] = NO_ZONE;

    PSend(TRAIN_CONTROLLER_MB, TRAIN_MONITOR_MB, msg_body, 3);

    // Reset triggered hall sensors in this train
    for (uint8_t i = 0; i < MAX_DIFF_SENSORS; i++) {
        trains_[train_num].triggered_sensors[i].reset = 0; // Reset both fields to 0
    }
}

// 255 for ERROR state (No train could possibly have triggered)
// 0 for Train 1,
// 1 for Train 2, etc.
uint8_t TrainController::WhichTrain(uint8_t hall_sensor_num) {
    uint8_t train_num = NO_TRAIN;

    std::stringstream tmp_debugging;
    tmp_debugging << "[TrainController::WhichTrain] Hall:" << (int)hall_sensor_num << "DIR: ";
    switch (trains_[1].train_ctrl.dir) {
        case CW:
            tmp_debugging << "CW";
            break;
        case CCW:
            tmp_debugging << "CCW";
            break;
        case STAY:
            tmp_debugging << "STAY";
            break;
    }

    /* OLD METHOD: Try this first */

    // Iterate through each train
    for(uint8_t x = 0; x < NUM_TRAINS; x++) {
        // If the train is not moving, it couldn't have triggered the hall sensor
        if (trains_[x].train_ctrl.dir == STAY) continue;

        // Iterate through each possible hall sensor for that zone and the train's current direction
        for (uint8_t y = 0; y < MAX_NUM_SENSORS_FROM_A_ZONE_FOR_A_DIRECTION; y++) {
            if (possible_halls[trains_[x].current_zone][trains_[x].train_ctrl.dir][y] == hall_sensor_num) {
                assert(train_num == NO_TRAIN); // Only 1 train should ever be able to hit a given sensor at a given time
                train_num = x;

                // The trains should be moving if it has triggered a hall sensor
                assert(trains_[train_num].train_ctrl.dir != STAY);
            }
        }
    }

    /* NEW METHOD: If the previous method fails, try the new one */

     uint8_t tmp_distance;
     uint8_t lowest_distance = 255;
     if (train_num == NO_TRAIN) {
       for(uint8_t x = 0; x < NUM_TRAINS; x++) {
            if (trains_[x].initialized == false || trains_[x].train_ctrl.dir == STAY) continue;
             // tmp_distance = zone_hall_distance_table[trains_[x].train_ctrl.dir][trains_[x].current_zone][hall_sensor_num];
             tmp_distance = 0;
             if (tmp_distance == lowest_distance) {
                std::cout << "WARNING: TWO TRAINS OF EQUAL DISTANCE TO A HALL SENSOR TRIGGER (NOW GAMBLING)\n";
            }
            else if (tmp_distance < lowest_distance) {
                train_num = x;
                lowest_distance = tmp_distance;
            }
        }
    }

    assert(train_num != NO_TRAIN);

    return train_num;
}

void TrainController::HandleZoneChange(uint8_t hall_sensor_num, uint8_t train_num) {
    // Determine which zone the train is moving into
    assert(trains_[train_num].train_ctrl.dir != STAY); // Ensure the train is actually moving

    // Store previous zone
    uint8_t prev_zone = trains_[train_num].current_zone;

    // Get new zone
    uint8_t tmp1_dir = (int)trains_[train_num].train_ctrl.dir;
    trains_[train_num].current_zone = possible_zones[hall_sensor_num][tmp1_dir];

    // If the new zone is ER (Error), the train is about to derail
    if (trains_[train_num].current_zone == ER) {
        StopTrain(train_num);
    }

    // Update monitor with zone changes
    static uint8_t msg_body[4];
    msg_body[0] = ZONE_CHANGE;
    msg_body[1] = train_num;
    msg_body[2] = trains_[train_num].current_zone;
    msg_body[3] = prev_zone;

    PSend(TRAIN_CONTROLLER_MB, TRAIN_MONITOR_MB, msg_body, 4);

    // Check if this is the train's destination
    if (trains_[train_num].current_dst == trains_[train_num].current_zone) StopTrain(train_num);
    else CheckIfRoutingNeeded(train_num);
}

// Maybe just collapse this into routing table itself, and check on every zone?

// If the train has entered a new zone, check if it should reroute from that zone
void TrainController::CheckIfRoutingNeeded(uint8_t train_num) {
    /* NEW METHOD */

    if (routing_table[trains_[train_num].current_zone][trains_[train_num].current_dst][1] == NOP_) return;

    // If in a switch zone, do not route
    for (uint8_t i = 0; i < MAX_NUM_SWITCHES; i++) {
        if (trains_[train_num].current_zone == switch_zones[i]) return;
    }

    RouteTrain(train_num);
}

// Handles sending train to any zone from any other zone
// Also in charge of stopping the train!
void TrainController::RouteTrain(uint8_t train_num, bool kick) {
    // Check if at current destination:
    if(trains_[train_num].stop_req || trains_[train_num].current_zone == trains_[train_num].current_dst) {
        // Stop train
        StopTrain(train_num);
        return;
    }

    /* Check if any switching needs to be done */

    switch_ctrl_t switch_ctrl;
    switch_ctrl.req_state = routing_table[trains_[train_num].current_zone][trains_[train_num].current_dst][1];

    // If switching is required, fetch the number and send the command *FIRST*
    if (switch_ctrl.req_state != NOT_NEEDED) {
        switch_ctrl.num = routing_table[trains_[train_num].current_zone][trains_[train_num].current_dst][2];
        assert(switch_ctrl.num != 0 && switch_ctrl.num <= 6); // TODO: For debugging

        SetSwitch(switch_ctrl);

        kick = true;
    }

    /* Route train as needed */

    // Fetch direction the train should go, storing the old direction for a later test
    switch_direction_t old_dir = (switch_direction_t)trains_[train_num].train_ctrl.dir;
    trains_[train_num].train_ctrl.dir = routing_table[trains_[train_num].current_zone][trains_[train_num].current_dst][0];

    // Stop the train if the routing table indicates that the train should be stopped
    if (trains_[train_num].train_ctrl.dir == STAY) { // If this happens, stop the train
        trains_[train_num].train_ctrl.speed = 0;
        CmdTrain(trains_[train_num].train_ctrl);
        return;
    }

    // Route the train if kicking or if the train has changed direction
    if (!kick && trains_[train_num].train_ctrl.dir == old_dir) return;

    trains_[train_num].train_ctrl.speed = trains_[train_num].default_speed;
    CmdTrain(trains_[train_num].train_ctrl);
}

void TrainController::HandleSensorTrigger(char* msg_body, uint8_t msg_len) {
    uint8_t sensor_num = (int)msg_body[1];

    // Determine which train triggered the sensor and that train's 
    uint8_t train_num = WhichTrain(msg_body[1]); // msg_body[1] contains the hall sensor #
    if (train_num == NO_TRAIN) {
        std::cout << "[TrainController::HandleSensorTrigger()] WARNING: No train could have triggered this >>" << (int)sensor_num << "<<\n";
        return;
    }

    /* Check if the sensor has been triggered recently and is already in triggered_sensors[] */
    uint8_t empty_idx = NO_INDEX;
    uint8_t oldest_idx = NO_INDEX;
    bool return_flag = false;
    for(uint8_t i = 0; i < MAX_DIFF_SENSORS; i++) {
        // Set empty index if one is found
        if (trains_[train_num].triggered_sensors[i].reset == 0) empty_idx = i;
        else if (trains_[train_num].triggered_sensors[i].sensor_num == sensor_num) {
            assert(trains_[train_num].triggered_sensors[i].num_triggers > 0); // Should have at least 1 trigger
            assert(return_flag == false); // For debugging, this should never trigger
            trains_[train_num].triggered_sensors[i].num_triggers += 1; // Iterate to another trigger
            // If two triggers have occurred, update train position into this zone
            if (trains_[train_num].triggered_sensors[i].num_triggers == 2) HandleZoneChange(sensor_num, train_num);
            // If more than 2 triggers have happened, just tick and return (Caused by double ticks)
            return_flag = true;
        }

        // Find the oldest while looping
        if (trains_[train_num].triggered_sensors[i].age == 3) oldest_idx = i;
    }

    // Return if the sensor was already in the list
    if(return_flag) return;

    /* If not in queue already, use the first empty or make an empty slot for it */

    // If full (Which should generally happen), remove the oldest entry and continue
    if (empty_idx == NO_INDEX) {
        assert(oldest_idx != NO_INDEX); // This means that an age of 3 was not found in the previous loop
        trains_[train_num].triggered_sensors[oldest_idx].reset = 0;
        empty_idx = oldest_idx;
    }

    assert(empty_idx != NO_INDEX); // This should really never happen
    assert(trains_[train_num].triggered_sensors[empty_idx].reset == 0); // TODO: Remove this debugging one

    trains_[train_num].triggered_sensors[empty_idx].sensor_num = sensor_num;
    trains_[train_num].triggered_sensors[empty_idx].num_triggers++;

    // Age all values that are reset
    for(uint8_t i = 0; i < MAX_DIFF_SENSORS; i++) {
        if (trains_[train_num].triggered_sensors[i].reset != 0) trains_[train_num].triggered_sensors[i].age++;
    }
}

void TrainController::TrainControllerLoop() {
    if (!PBind(TRAIN_CONTROLLER_MB, BIG_LETTER)) { // Default mailbox size of 16, which is MAX_ALARMS*2
        std::cout << "TrainController::Initialize: WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TrainController::TrainControllerLoop: WARNING Mailbox bound!\n";

    SingletonGrab();

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[10]; // TODO: Magic number

    static switch_ctrl_t switch_ctrl;
    static train_ctrl_t train_ctrl;

    uint8_t tmp_num;

    // Reset switches to straight on startup
    TrainCommandCenterInstance_->SendSwitchCommand(255, STRAIGHT, TRAIN_CONTROLLER_MB);

    // Should just be receiving from UART and Data Link Layer
    while(1) {
        PRecv(src_q, TRAIN_CONTROLLER_MB, &msg_body, mailbox_msg_len);

        // TODO: Functionize this table
        switch (msg_body[0]) {
            case '\xA0': // Hall sensor (#) triggered
                // Update state information
                HandleSensorTrigger(msg_body, mailbox_msg_len);
                break;
            case '\xC2': // Train ACK
                // Fetch zero indexed number of train
                tmp_num = msg_body[1] - 1;

                CmdTrain(last_train_cmds[tmp_num]); // Will re-buffer and all that


                /* OLD METHOD
                if (train_msg_buffer_->Empty()) {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: ACK Was receieved for unaccounted for TRAIN command\n";
                    break;
                }
                else train_ctrl = train_msg_buffer_->Get();

                tmp_num = msg_body[1] - 1;
                assert(train_ctrl.num == tmp_num); // Top of ring buffer's switch num should equal the ACKed request

                // Check if msg was successful or not
                if (msg_body[2] == 0) { // If success, update state information
                    // This never happens currently
                }
                else {
                    // std::cout << "[TrainController::TrainControllerLoop] WARNING: TRAIN Command failed, resending request\n";
                    std::cout << "RT\n";
                    CmdTrain(train_ctrl); // Will re-buffer and all that
                }
                */

                break;
            case '\xE2': // Switch ACK
                if (switch_msg_buffer_->Empty()) {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: ACK Was receieved for unaccounted for SWITCH command\n";
                    break;
                }
                else switch_ctrl = switch_msg_buffer_->Get();

                assert(switch_ctrl.num == msg_body[1]); // Top of ring buffer's switch num should equal the ACKed request

                // Check if msg was successful or not
                if (msg_body[2] == 0) { // If success, update state information
                    // This never happens currently
                }
                else {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: SWITCH Command failed, resending request\n";
                    SetSwitch(switch_ctrl);
                }

                break;
            case ZONE_CMD: // Internal Init cmd
                // Only allowed to initialize train while it is not en route (signified by dst == NO_ZONE)
                if (trains_[msg_body[1]].initialized == false) {
                    trains_[msg_body[1]].initialized = true;

                    assert(trains_[msg_body[1]].train_ctrl.dir == STAY); // If train is not en_route, it should be stationary
                    trains_[msg_body[1]].current_zone = msg_body[2];

                    // Update monitor with zone changes
                    msg_body[0] = ZONE_CHANGE;
                    // msg_body[1] = msg_body[1]; // Already in place
                    // msg_body[2] = msg_body[2];
                    msg_body[3] = NO_ZONE;

                    PSend(TRAIN_CONTROLLER_MB, TRAIN_MONITOR_MB, msg_body, 4);
                }
                break;

            case TRAIN_GO_CMD:
                // msg_body[1] contains the train number
                // msg_body[2] contains the first destination

                if(trains_[msg_body[1]].initialized == true) {
                    // If the train is currently stationary and not en_route
                    // TODO: Add functionality to allow the train to be re-routed when en-route
                    if (trains_[msg_body[1]].train_ctrl.dir == STAY && trains_[msg_body[1]].current_dst == NO_ZONE) {
                        trains_[msg_body[1]].current_dst = msg_body[2];
                        RouteTrain(msg_body[1], true);

                        // Update Monitor with new Destination
                        PSend(TRAIN_CONTROLLER_MB, TRAIN_MONITOR_MB, msg_body, 3);
                    }
                }
                break;
            case KICK_CMD:
                if(trains_[msg_body[1]].initialized == true) {
                    RouteTrain(msg_body[1], true);
                }
                break;
            case SUDO_STOP_CMD:
                assert(msg_body[1] != 0 && msg_body[1] <= NUM_TRAINS);
                StopTrain(msg_body[1], false);
                break;
            case STOP_CMD:
                assert(msg_body[1] != 0 && msg_body[1] <= NUM_TRAINS);
                trains_[msg_body[1]].stop_req = true;
                break;
            default:
                std::cout << "TrainController::TrainControllerLoop(): ERROR: Invalid COMMAND >>" << HEX(msg_body[0]) << "<<\n";
                while (1) {}
                break;
        }
    }
}

void TrainController::SingletonGrab() {
    TrainCommandCenterInstance_ = TrainCommandCenter::GetTrainCommandCenter();
    TrainMonitorInstance_ = TrainMonitor::GetTrainMonitor();
}

/*
    Function: GetTrainController
    Output: CommandCenterInstance_: Pointer to the TrainController Singleton
    Brief: Get function for the TrainController singleton
*/
TrainController* TrainController::GetTrainController() {
    if (!TrainControllerInstance_) TrainControllerInstance_ = new TrainController;
    return TrainControllerInstance_;
}
