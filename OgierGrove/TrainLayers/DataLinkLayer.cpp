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
-> Name:  DataLinkLayer.cpp
-> Date: Nov 16, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/DataLinkLayer.h"

// TODO: Delete?
#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>

#include "Includes/TrainMonitor.h"

// Singleton Instance
DataLinkLayer *DataLinkLayer::DataLinkLayerInstance_ = 0;

// Note: The pre-infinite loop of this acts as an initialization sequence
void DataLinkLayer::MailboxLoop() {
    // Bind PhysicalLayer queue
    if (PBind(DATA_LINK_LAYER_MB, SMALL_LETTER) == false) { // Default mailbox size of 16
        std::cout << "DataLinkLayer::MailboxLoop(): WARNING Mailbox failed to bind\n";
    }
    else std::cout << "DataLinkLayer::MailboxLoop(): Mailbox bound\n";

    uint8_t src_q;
    uint32_t mailbox_msg_len;
    char msg_body[SMALL_LETTER];

    // initialize to 0
    num_packets_in_limbo_ = 0;
    tiva_ns_ = 0;
    tiva_nr_ = 0;

    // Initialize all control blocks of the outgoing_messages_ queue to UNUSED_PT
    for(int i = 0; i < MAX_DLL_WAITING_PACKETS; i++) outgoing_messages_[i].control_block.type = UNUSED_PT;

    packet_t* recv_packet_ptr;
    packet_t newly_made_packet;

    while (1) {

        // Blocking message request
        if (!PRecv(src_q, DATA_LINK_LAYER_MB, &msg_body, mailbox_msg_len)) {
            std::cout << "DataLinkLayer::MailboxLoop(): WARNING! PRecv resulted in failure!\n";
        }
        
        // Error state checking for testing
        assert(mailbox_msg_len < SMALL_LETTER);

        switch(src_q) {
            // If from the physical layer, handle each packet type separately
            case UART_PHYSICAL_LAYER_MB:
                // Cast message to the correct format
                recv_packet_ptr = (packet_t *)msg_body;

                switch(recv_packet_ptr->control_block.type) {
                    case DATA_PT:
                        assert(mailbox_msg_len >= 3 && mailbox_msg_len <= 5);
                        assert(recv_packet_ptr->length <= 3);

                        // Ensure ns/nr is correct, drop and send NACK if necessary
                        if (recv_packet_ptr->control_block.ns == tiva_nr_) {
                            // If valid, send packet to application layer
                            SendMessageUp(recv_packet_ptr); // This increments the tiva_nr_
                            // Increment NR as message has been sent up
                            MOD8PLUS1(tiva_nr_);
                            // Check for piggybacking ACKS
                            HandleACK(recv_packet_ptr->control_block.nr); // TODO: TEST THIS WORKS
                            // ACK the packet with the incremented NR
                            SendACK();
                        }
                        else SendNACK();
                        break;
                    case ACK_PT:
                        assert(mailbox_msg_len == 1 || mailbox_msg_len == 2); // Apparently the length field is present but zero?
                        HandleACK(recv_packet_ptr->control_block.nr);
                        break;
                    case NACK_PT:
                        assert(mailbox_msg_len == 1 || mailbox_msg_len == 2); // Apparently the length field is present but zero?
                        HandleNACK(recv_packet_ptr->control_block.nr);
                        break;
                    default:
                        std::cout << "  DataLinkLayer::MailboxLoop(): ERROR: INVALID PHYSICAL LAYER TYPE\n";
                        while(1) {}
                        // TODO: Error case
                        break;
                }
                break;
            // If from the time server, a message has possibly timed out and needs to be handled
            case TRAIN_TIME_SERVER_MB:
                // Should always just get a single char from the train server
                assert(mailbox_msg_len == 1);

                recv_packet_ptr = &outgoing_messages_[*((uint8_t *)msg_body)];

                // assert(recv_packet_ptr->control_block.type == DATA_PT);

                // Resend packet
                PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, (void *)recv_packet_ptr, recv_packet_ptr->length + 2);

                #if DEBUGGING_TRAIN >= 1
                PSend(DATA_LINK_LAYER_MB, MONITOR_MB, (void *)recv_packet_ptr, recv_packet_ptr->length + 2);
                #endif

                // Set another alarm to resend it again if necessary
                SetPacketAlarm(recv_packet_ptr->control_block.ns);

                break;
            // Else it is from the application layer and should be sent down to the Physical Layer
            default:

                // Make packet for sending or buffering
                assert(mailbox_msg_len < 8);
                MakePacket(newly_made_packet, (train_msg_t *)msg_body);

                // Add to outgoing mail buff if allowed
                if (num_packets_in_limbo_ < WINDOW_SIZE) {
                    SendPacketDown(&newly_made_packet);

                    // Increment NS after this gets sent
                    MOD8PLUS1(tiva_ns_);
                }
                // Else, buffer the msg for later
                else {
                    if (!packet_buffer_->Full()) {
                        // Make packet to send
                        packet_buffer_->Add(&newly_made_packet);

                        // Increment NS as this gets buffered
                        MOD8PLUS1(tiva_ns_);
                    }
                    // TODO: Not an error state, but shouldn't really happen so this is here for testing
                    else std::cout << "  DataLinkLayer::MailboxLoop(): WARNING, DATA BUFFER FULL AND PACKET LOST\n";
                }
                break;
        }
    }
}

// Send message up to application layer
void DataLinkLayer::SendMessageUp(packet_t* packet) {
    assert(packet->length >= 1 && packet->length <= 3);

    // Strip out message itself and send up to the application layer
    static char msg_body[SMALL_LETTER];

    switch(packet->length) {
        // Note: The lack of break statements is intentional for a waterfall effect
        case 3:
            msg_body[2] = packet->arg2;
        case 2:
            msg_body[1] = packet->arg1;
        case 1:
            msg_body[0] = packet->code;
            break;
        default:
            break;

    }

    PSend(DATA_LINK_LAYER_MB, TRAIN_APPLICATION_LAYER_MB, (void *)msg_body, packet->length);
}

// Send packet down to application layer
void DataLinkLayer::SendPacketDown(packet_t* packet) {
    // Add packet to outgoing_messages_ buffer
    assert(packet->length + 2 <= MAX_PACKET_SIZE); // Check that memcopy isn't going to clobber other data
    memcpy(&outgoing_messages_[tiva_ns_], packet, packet->length + 2);

    // This assumes that num_packets_in_limbo_ was checked for bounds before calling this function 
    num_packets_in_limbo_++;

    assert(packet->control_block.type == DATA_PT);
    PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, (void *)packet, packet->length + 2);

    SetPacketAlarm(packet->control_block.ns);

    static std::stringstream sstream;
    if (packet->code != '\xA2') {
        for(uint8_t i = 0; i < (packet->length + 2); i++) sstream << HEX(packet->tmp_array[i]);
        TrainMonitor::GetTrainMonitor()->VisuallyDisplayTX(sstream.str());
    }
    sstream.str("");
    sstream.clear();

    #if DEBUGGING_TRAIN >= 1
    packet->tmp_array[packet->length + 2] = num_packets_in_limbo_;
    PSend(DATA_LINK_LAYER_MB, MONITOR_MB, (void *)packet, packet->length + 3);
    #endif
}

// Clear appropriate message from buffer and update 'num_packets_in_limbo_' number
void DataLinkLayer::HandleACK(uint8_t train_nr) {

    // All ACKs should be in the range, otherwise the ATMega is ACKting up
    assert(CheckACKRange(train_nr)); // If found to be outside the range, that's a problem

    static packet_t packet;

    // Clear any packets in limbo within sliding window size backwards
    // if (outgoing_messages_[train_nr].control_block.type != DATA_PT) return;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        // If out of packets, break
        if (num_packets_in_limbo_ == 0) break;
        MOD8MINUS1(train_nr);

        // If there are no more packets in this range, break
        if (outgoing_messages_[train_nr].control_block.type == UNUSED_PT) break;
        // assert(outgoing_messages_[train_nr].control_block.type == UNUSED_PT);

        // Clear alarm, lower limbo_num, and clear buffer spot
        InternallyACK(train_nr);
    }

    // Check if any other messages are waiting to be sent
    while(num_packets_in_limbo_ < WINDOW_SIZE) {
        if (packet_buffer_->Empty()) break;
        packet = packet_buffer_->Get();
        SendPacketDown(&packet); // This increments num_packets_in_limbo_
    }
}

// Resend all packets stuck in limbo, starting with the NACK value and continuing until
// the sliding window is satisfied or until a UNUSED_PT is found
void DataLinkLayer::HandleNACK(uint8_t train_nr) {
    MOD8MINUS1(train_nr);

    assert(train_nr <= MAX_DLL_WAITING_PACKETS);

    // Check that the message is in the outgoing_message array
    assert(outgoing_messages_[train_nr].control_block.type == DATA_PT);

    // Resend message and any other message in sliding window
    // TODO: Maybe redo this with a cross-reference to 'num_packets_in_limbo_'
    for (int i = 0, msg_idx = train_nr; i < WINDOW_SIZE; i++, MOD8PLUS1(msg_idx)) {
        assert(msg_idx >= 0 && msg_idx < MAX_DLL_WAITING_PACKETS);
        if (outgoing_messages_[msg_idx].control_block.type != DATA_PT) break;
        PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, &outgoing_messages_[msg_idx], outgoing_messages_[msg_idx].length + 2);
    }
}

// Send an ACK to acknowedge we receieved and accepted their message
void DataLinkLayer::SendACK() {
    // TODO: Get initializer to set the starting value to NACK_PT
    static control_t control_block;

    control_block.type = ACK_PT;
    control_block.ns = 0;
    control_block.nr = tiva_nr_;

    PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, &control_block, 1);

    #if DEBUGGING_TRAIN >= 1
    PSend(DATA_LINK_LAYER_MB, MONITOR_MB, &control_block, 1);
    #endif
}

// Send a nack to request the message we were expecting,
// this is called if we get a message we were not expecting (Trainset NS didn't equal TIVA NR)
void DataLinkLayer::SendNACK() {
    // TODO: Get initializer to set the starting value to NACK_PT
    static control_t control_block;

    control_block.type = NACK_PT;
    control_block.ns = 0; // TODO: 0 this. Was changed for testing.
    control_block.nr = tiva_nr_;

    PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, &control_block, 1);

    #if DEBUGGING_TRAIN >= 1
    PSend(DATA_LINK_LAYER_MB, MONITOR_MB, &control_block, 1);
    #endif
}

void DataLinkLayer::InternallyACK(uint8_t buffer_idx) {
    // This should be set, this *should* not be clearing things that are not set
    assert(outgoing_messages_[buffer_idx].control_block.type == DATA_PT);

    // 1) Clear alarm
    SetPacketAlarm(buffer_idx, false);

    // 2) Clear buffer at index
    outgoing_messages_[buffer_idx].control_block.type = UNUSED_PT;

    // 3) Lower Limbo Number
    num_packets_in_limbo_--;

    // TODO: (Maybe) If num_packets_in_limbo_, could do a hard reset of all packets in the table.
    // and alarms in timeserver. Very costly, but could prevent issues
}

void DataLinkLayer::SetPacketAlarm(uint8_t alarm_num, bool set_flag) {
    // Inform time server that this message has been buffered
    train_alarm_t alarm_msg;
    alarm_msg.ns = alarm_num;
    alarm_msg.set_flag = set_flag;
    PSend(DATA_LINK_LAYER_MB, TRAIN_TIME_SERVER_MB, &alarm_msg, 2);
}

bool DataLinkLayer::CheckACKRange(uint8_t incoming_nr) {
    // Check upper limit
    if (incoming_nr == tiva_ns_) return true;

    // Check down to lower limit
    uint8_t tmp_ns = tiva_ns_;
    for(uint8_t i = 0; i < WINDOW_SIZE; i++) {
        MOD8MINUS1(tmp_ns);
        if (incoming_nr == tmp_ns) return true;
    }

    std::cout << "DataLinkLayer::CheckACKRange(): FAILURE >>NS: " << (int)tiva_ns_ << "<< >>NR " 
                                                                  << (int)incoming_nr << "<< >>Limbo: "
                                                                  << (int)num_packets_in_limbo_ << "<<\n";
    return false;
}

void DataLinkLayer::MakePacket(packet_t &packet, train_msg_t* msg) {
    // Set control block
    packet.control_block.type = DATA_PT;
    packet.control_block.nr = tiva_nr_;
    packet.control_block.ns = tiva_ns_;

    // Set Msg
    packet.msg.code = msg->code;

    // TODO: Maybe optimize this abit, the double switch statements are a bit much

    // Set length
    packet.length = MsgLengthFromCode(packet.msg.code);

    switch(packet.length) {
        // Note: The lack of break statements is intentional for a waterfall effect
        case 3:
            packet.arg2 = msg->arg2;
        case 2:
            packet.arg1 = msg->arg1;
        case 1:
            break;
        default:
            std::cout << "  DataLinkLayer::MakePacket(): ERROR, Invalid Packet Length\n";
            while(1) {}
            break;
    }
}

/*
    Function: DataLinkLayer
    Brief: Constructor for the DataLinkLayer class
*/
DataLinkLayer::DataLinkLayer() {
    packet_buffer_ = new RingBuffer<packet_t>(PACKET_BUFFER_SIZE);
}

/*
    Function: ~DataLinkLayer
    Brief: Destructor for the DataLinkLayer class
*/
DataLinkLayer::~DataLinkLayer() {
    delete packet_buffer_;
}

/*
    Function: GetDataLinkLayer
    Output: DataLinkLayerInstance_: Pointer to the DataLinkLayer Singleton
    Brief: Get function for the DataLinkLayer singleton
*/
DataLinkLayer* DataLinkLayer::GetDataLinkLayer() {
    if (!DataLinkLayerInstance_) DataLinkLayerInstance_ = new DataLinkLayer;
    return DataLinkLayerInstance_;
}
