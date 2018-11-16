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
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/DataLinkLayer.h"

// Singleton Instance
DataLinkLayer *DataLinkLayer::DataLinkLayerInstance_ = 0;

// Note: The pre-infinite loop of this acts as an initialization sequence
void DataLinkLayer::MailboxLoop() {
    // Bind PhysicalLayer queue
    PBind(DATA_LINK_LAYER_MB, ONE_CHAR); // Default mailbox size of 16

    uint8_t src_q;
    uint32_t mailbox_msg_len;
    char msg_body[256];

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
        PRecv(src_q, DATA_LINK_LAYER_MB, &msg_body, mailbox_msg_len);
        
        // Error state checking for testing
        assert(mailbox_msg_len < 256);

        switch(src_q) {
            // If from the physical layer, handle each packet type separately
            case PACKET_PHYSICAL_LAYER_MB:
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
                            // ACK the packet with the incremented NR
                            SendACK();
                        }
                        else SendNACK();
                        break;
                    case ACK_PT:
                        assert(mailbox_msg_len == 1);
                        HandleACK(recv_packet_ptr->control_block.nr);
                        break;
                    case NACK_PT:
                        assert(mailbox_msg_len == 1);
                        HandleNACK(recv_packet_ptr->control_block.nr);
                        break;
                    default:
                        // TODO: Error case
                        break;
                }
                break;
            /* TODO: Time server && associated timeouts
            // If from the time server, a message has possibly timed out and needs to be handled
            case TIME_SERVER_MB:
                // NOTE: Time server should expect two types of messages:
                    // 1: Request for timer to be set up for mod8 packet number
                    // 2: Request for timer to be cancelled for mod8 packet number
                    // Therefore, these replies *should* ALL be for packets that have actually been missed

                // Check which one timed out and resend
                break;
            */
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
                    else std::cout << "DataLinkLayer::MailboxLoop(): WARNING, DATA BUFFER FULL AND PACKET LOST\n";
                }
                break;
        }
    }
}

// Send message up to application layer
void DataLinkLayer::SendMessageUp(packet_t* packet) {
    assert(packet->length <= 1 && packet->length >= 3);

    // Strip out message itself and send up to the application layer
    static char msg_body[256];

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

    PSend(DATA_LINK_LAYER_MB, APPLICATION_LAYER_MB, (void *)msg_body, packet->length);
}

// Send packet down to application layer
void DataLinkLayer::SendPacketDown(packet_t* packet) {
    // Add packet to outgoing_messages_ buffer
    memcpy(&outgoing_messages_[tiva_ns_], packet, packet->length + 2);

    // This assumes that num_packets_in_limbo_ was checked for bounds before calling this function 
    num_packets_in_limbo_++;

    PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, (void *)packet, packet->length + 2);
}

// Clear appropriate message from buffer and update 'num_packets_in_limbo_' number
void DataLinkLayer::HandleACK(uint8_t train_nr) {
    static packet_t packet;

    // Clear data from table by setting type to UNUSED_PT
    assert(outgoing_messages_[train_nr].control_block.type == DATA_PT);
    outgoing_messages_[train_nr].control_block.type = UNUSED_PT;

    // Update num_packets_in_limbo_ number
    num_packets_in_limbo_--;

    // Check if any other messages are waiting to be sent
    if (!packet_buffer_->Empty()) {
        packet = packet_buffer_->Get(); // TODO: Avoid this copying? Idk how right now
        SendPacketDown(&packet);
    }
}

// Resend all packets stuck in limbo, starting with the NACK value and continuing until
// the sliding window is satisfied or until a UNUSED_PT is found
void DataLinkLayer::HandleNACK(uint8_t train_nr) {
    // Check that the message is in the outgoing_message array
    assert(outgoing_messages_[train_nr].control_block.type == DATA_PT);

    // Resend message and any other message in sliding window
    // TODO: Maybe redo this with a cross-reference to 'num_packets_in_limbo_'
    for (int i = 0, msg_idx = train_nr; i < WINDOW_SIZE; i++, MOD8PLUS1(msg_idx)) {
        if (outgoing_messages_[msg_idx].control_block.type != DATA_PT) break;
        PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, &outgoing_messages_[msg_idx], outgoing_messages_[msg_idx].length + 2);
    }
}

// Send an ACK to acknowedge we receieved and accepted their message
void DataLinkLayer::SendACK() {
    // TODO: Get initializer to set the starting value to NACK_PT
    static control_t control_block;

    control_block.type = ACK_PT;
    control_block.ns = tiva_ns_;
    control_block.nr = tiva_nr_;

    PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, &control_block, 1);

}

// Send a nack to request the message we were expecting,
// this is called if we get a message we were not expecting (Trainset NS didn't equal TIVA NR)
void DataLinkLayer::SendNACK() {
    // TODO: Get initializer to set the starting value to NACK_PT
    static control_t control_block;

    control_block.type = NACK_PT;
    control_block.ns = tiva_ns_;
    control_block.nr = tiva_nr_;

    PSend(DATA_LINK_LAYER_MB, PACKET_PHYSICAL_LAYER_MB, &control_block, 1);
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
    packet.length = PacketLengthFromCode(packet.msg.code);

    switch(packet.length) {
        // Note: The lack of break statements is intentional for a waterfall effect
        case 3:
            packet.arg2 = msg->arg2;
        case 2:
            packet.arg1 = msg->arg1;
        case 1:
            break;
        default:
            std::cout << "DataLinkLayer::MakePacket(): ERROR, Invalid Packet Length\n";
            while(1) {}
            break;

    }
}

uint8_t DataLinkLayer::PacketLengthFromCode(uint8_t msg_code) {
    switch (msg_code) {
        case '\xA8':
            return 1;
        case '\xA2':
            return 2;
        case '\xC0':
        case '\xE0':
            return 3;
        default:
            std::cout << "DataLinkLayer::PacketLengthFromCode(): ERROR, Invalid Message code\n";
            while(1) {}
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

void DataLinkLayer::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
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
