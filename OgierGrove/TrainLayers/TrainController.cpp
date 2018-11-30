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
    train_msg_buffer_ = new RingBuffer<train_ctrl_t>(TRAIN_BUFFER_SIZE);

    for(uint8_t i = 0; i < NUM_TRAINS; i++) {
        trains_[i].dir = STAY;
        trains_[i].primary_zone = NO_ZONE;
        trains_[i].secondary_zone = NO_ZONE;
        trains_[i].last_hall_triggered = NO_HALL;
        trains_[i].second_last_hall_triggered = NO_HALL;
        trains_[i].current_dst = NO_ZONE;
        trains_[i].speed = 4; // TODO: Un-hardcode this speed

        trains_[i].num = i + 1;
        train
    }
}

TrainController::~TrainController() {
    delete switch_buffer_;
    delete train_msg_buffer_;
}

void TrainController::SetSwitch(switch_ctrl_t switch_ctrl) {
    switch_buffer_->Add(switch_ctrl);
    TrainCommandCenterInstance_->SendSwitchCommand(switch_ctrl.switch_num, switch_ctrl.req_state, TRAIN_CONTROLLER_MB);
}

void TrainController::CmdTrain(train_ctrl_t train_ctrl) {
    train_msg_buffer_->Add(train_ctrl);
    TrainCommandCenterInstance_->SendTrainCommand(train_ctrl.num, train_ctrl.req_speed, train_ctrl.req_dir, TRAIN_CONTROLLER_MB);
}

// 0 for ERROR state (No train could possibly have triggered)
// 1 for Train 1, etc.
uint8_t TrainController::WhichTrain(uint8_t hall_sensor_num) {
    uint8_t train_num = 0;

    // Iterate through each train
    for(uint8_t x = 0; x < NUM_TRAINS; x++) {
        // If the train is not moving, it couldn't have triggered the hall sensor
        if (trains_[x].dir == STAY) continue;

        // Iterate each zone for the train (Primary/Secondary)
        for (uint8_t y = 0; y < MAX_ZONES_PER_TRAIN; y++) {

            // Iterate through each possible hall sensor for that zone and the train's current direction
            for (uint8_t z = 0; z < MAX_NUM_SENSORS_FROM_A_ZONE_FOR_A_DIRECTION; z++) {
                if (possible_halls[trains_[x].zones[y]][trains_[x].dir][z] == hall_sensor_num) {
                    assert(train_num == 0); // Only 1 train should ever be able to hit a given sensor at a given time
                    train_num = x;

                    // The trains should be moving if it has triggered a hall sensor
                    assert(trains_[train_num].dir != STAY);
                }
            }
        }
    }

    return train_num;
}

void TrainController::HandleZoneChange(uint8_t hall_sensor_num, uint8_t train_num) {
    // Determine which zone the train is moving into

    static uint8_t msg_body[4];

    assert(trains_[train_num].dir != STAY); // Ensure the train is actually moving
    uint8_t new_zone = possible_zones[hall_sensor_num][trains_[train_num].dir][1];
    uint8_t prev_zone = possible_zones[hall_sensor_num][(trains_[train_num].dir == CW) ? CW : CCW][1]; // Previous zone is the zone that we were

    assert(new_zone != ER); // TODO: make this stop all trains

    // Make the new zone the primary zone
    trains_[train_num].primary_zone = new_zone;

    // Update monitor with zone changes
    msg_body[0] = ZONE_CHANGE;
    msg_body[1] = train_num;
    msg_body[2] = new_zone;
    msg_body[3] = prev_zone;

    PSend(TRAIN_CONTROLLER_MB, TRAIN_MONITOR_MB, msg_body, 4);

    // Route if needed
    CheckIfRoutingNeeded(train_num);
}

// Maybe just collapse this into routing table itself, and check on every zone?

// If the train has entered a new zone, check if it should reroute from that zone
void TrainController::CheckIfRoutingNeeded(uint8_t train_num) {
    for (uint8_t i = 0; i < NUM_DANGER_ZONES; i++) {
        if ((trains_[train_num].primary_zone == routing_needed_zones[i][0]) && (trains_[train_num].dir == routing_needed_zones[i][1])) {
            RouteTrain(train_num);
            return;
        }
    }
}

// Handles sending train to any zone from any other zone
// Also in charge of stopping the train!
void TrainController::RouteTrain(uint8_t train_num) {
    // Fetch direction the train should go
    train_ctrl_t train_ctrl;
    train_ctrl.num = train_num;
    train_ctrl.req_dir = routing_table[trains_[train_num].primary_zone][trains_[train_num].current_dst][0];
    train_ctrl.

    // Check if any switching needs to be done
    switch_ctrl_t switch_ctrl;
    switch_ctrl.req_state = routing_table[trains_[train_num].primary_zone][trains_[train_num].current_dst][1];

    // If switching is required, fetch the number and send the command *FIRST*
    if (switch_ctrl.req_state != NOT_NEEDED) {
        switch_ctrl.num = routing_table[trains_[train_num].primary_zone][trains_[train_num].current_dst][2];
        assert(switch_num != 0 && switch_num <= 6); // TODO: For debugging

        SetSwitch(switch_ctrl);
    }

    CmdTrain(train_num);
}

void TrainController::HandlePartialZoneChange(uint8_t hall_sensor_num, uint8_t train_num) {
    static uint8_t msg_body[3];

    assert(trains_[train_num].dir != STAY); // Ensure the train is actually moving
    uint8_t partial_zone = possible_zones[hall_sensor_num][trains_[train_num].dir][1];

    // Update monitor with zone changes
    msg_body[0] = ZONE_CHANGE;
    msg_body[1] = train_num;
    msg_body[2] = partial_zone;

    PSend(TRAIN_CONTROLLER_MB, TRAIN_MONITOR_MB, msg_body, 3);
}

void TrainController::TrainControllerLoop() {
    if (!PBind(TRAIN_CONTROLLER_MB, BIG_LETTER)) { // Default mailbox size of 16, which is MAX_ALARMS*2
        std::cout << "TrainController::Initialize: WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TrainController::TrainControllerLoop: WARNING Mailbox bound!\n";

    SingletonGrab();

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[2]; // TODO: Magic number

    static switch_ctrl_t switch_ctrl;
    static train_ctrl_t train_ctrl;

    uint8_t tmp_num;

    static uint8_t zone_change_msg[4];
    zone_change_msg[0] = ZONE_CHANGE;

    // Should just be receiving from UART and Data Link Layer
    while(1) {
        PRecv(src_q, TRAIN_CONTROLLER_MB, &msg_body, mailbox_msg_len);

        // TODO: Functionize this table
        switch (msg_body[0]) {
            case '\xA0': // Hall sensor (#) triggered
                // Update state information

                // Determine which sensor was triggered
                tmp_num = WhichTrain(msg_body[1]); // msg_body[1] contains the hall sensor #

                /* If either one has triggered this sensor recently */

                if (trains_[tmp_num].last_hall_triggered == msg_body[1]) {
                    trains_[tmp_num].last_hall_triggered = NO_HALL;

                    // Second one should be zero
                    if (trains_[tmp_num].second_last_hall_triggered != NO_HALL) {
                        std::cout << "[TrainController::TrainControllerLoop()] WARNING: This should be NO_HALL\n";
                        trains_[tmp_num].second_last_hall_triggered = NO_HALL;
                    }

                    HandleZoneChange(msg_body[1], tmp_num);
                    break;
                }
                else if (trains_[tmp_num].second_last_hall_triggered == msg_body[1]) {
                    trains_[tmp_num].second_last_hall_triggered = NO_HALL;
                    HandleZoneChange(msg_body[1], tmp_num);
                    break;
                }

                /* Else, Fill buffers accordingly */

                // Second one should be zero here
                if (trains_[tmp_num].second_last_hall_triggered != NO_HALL) {
                    std::cout << "[TrainController::TrainControllerLoop()] WARNING: This should be NO_HALL\n";
                    trains_[tmp_num].second_last_hall_triggered = NO_HALL;
                }   

                // Else, fill buffer accordingly
                if (trains_[tmp_num].last_hall_triggered == NO_HALL) {
                    trains_[tmp_num].last_hall_triggered = msg_body[1];
                }
                else {
                    // If first one is filled, make it second and fill first one with current value
                    trains_[tmp_num].second_last_hall_triggered = trains_[tmp_num].last_hall_triggered;
                    trains_[tmp_num].last_hall_triggered = msg_body[1];
                }

                // This means the train is transitioning between two zones
                HandlePartialZoneChange(msg_body[1], tmp_num);

                break;
            case '\xC2': // Train ACK
                if (train_msg_buffer_->Empty()) {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: ACK Was receieved for unaccounted for TRAIN command\n";
                    break;
                }
                else train_ctrl = train_buffer_->Get();

                tmp_num = msg_body[1];
                assert(train_ctrl.num == tmp_num); // Top of ring buffer's switch num should equal the ACKed request

                // Check if msg was successful or not
                if (msg_body[2] == 0) { // If success, update state information
                    // This never happens currently
                }
                else {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: TRAIN Command failed, resending request\n";
                    CmdTrain(train_ctrl); // Will re-buffer and all that
                }

                break;
            case '\xE2': // Switch ACK
                if (switch_msg_buffer_->Empty()) {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: ACK Was receieved for unaccounted for SWITCH command\n";
                    break;
                }
                else switch_ctrl = switch_buffer_->Get();

                assert(switch_ctrl.num == msg_body[1]); // Top of ring buffer's switch num should equal the ACKed request

                // Check if msg was successful or not
                if (msg_body[2] == 0) { // If success, update state information
                    // This never happens currently
                }
                else {
                    std::cout << "[TrainController::TrainControllerLoop] WARNING: SWITCH Command failed, resending request\n";
                    TrainCommandCenterInstance_->SendSwitchCommand(switch_ctrl.switch_num, switch_ctrl.req_state, TRAIN_CONTROLLER_MB);
                }

                break;
            case ZONE_CMD: // Internal Init cmd
                // Only allowed to initialize train while it is not en route (signified by dst == NO_ZONE)
                if (trains_[msg_body[1]].current_dst == NO_ZONE) {
                    assert(msg_body[1].dir == STAY); // If train is not en_route, it should be stationary
                    trains_[msg_body[1]].primary_zone = msg_body[2];

                }
                break;
            case TRAIN_GO_CMD:
                // msg_body[1] contains the train number
                // msg_body[2] contains the first destination

                // If the train is currently stationary and not en_route
                // TODO: Add functionality to allow the train to be re-routed when en-route
                if (trains_[msg_body[1]].dir == STAY && trains_[msg_body[1]].current_dst == NO_ZONE) {
                    trains_[msg_body[1]].current_dst = msg_body[2];
                    RouteTrain(msg_body[1]);
                }
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
