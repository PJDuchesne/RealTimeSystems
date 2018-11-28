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
-> Name:  PhysicalLayer.cpp
-> Date: Nov 16, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/PhysicalLayer.h"

// TODO: Delete, used for testing
#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>

// Singleton Instance
PhysicalLayer *PhysicalLayer::PhysicalLayerInstance_ = 0;

/*
    Function: UARTMailboxLoop
    Brief: <>
*/
void PhysicalLayer::UARTMailboxLoop() {
    // Bind PhysicalLayer queue
    if (!PBind(UART_PHYSICAL_LAYER_MB, ONE_CHAR, UART_PHYSICAL_LAYER_MB_SIZE)) {
        std::cout << "PhysicalLayer::UARTMailboxLoop(): WARNING Mailbox failed to bind!\n";
    }
    else std::cout << "PhysicalLayer::UARTMailboxLoop(): Mailbox bound\n";

    // Grab singleton for future use
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body;
    static unsigned char incoming_frame[MAX_FRAME_SIZE];
    static uint8_t frame_len = 0;

    // Hijacking function for testing without the trainset
    // #if DEBUGGING_TRAIN >= 1
    // int i_DELAY = 0;
    
    // while (1) {
    //     // Short delay
    //     DELAY(1000000/4)

    //     // Send a dummy packet to test system
    //     FakePacket();
    // }
    // #endif

    while (1) {
        // Blocking message request
        PRecv(src_q, UART_PHYSICAL_LAYER_MB, &msg_body, mailbox_msg_len);

        // TODO: Delete debugging statement
        std::cout << "    PhysicalLayer::UARTMailboxLoop(): CHAR>>" << HEX(msg_body) << "<<\n";
        PSend(UART_PHYSICAL_LAYER_MB, MONITOR_MB, (void *)msg_body, mailbox_msg_len);

        // Error state checking for testing
        assert(mailbox_msg_len == 1);
        assert(frame_len < MAX_FRAME_SIZE);

        // Ignore all characters until a 0x02 is sent to start the frame
        if(frame_len == 0 && msg_body != '\x02') continue;

        incoming_frame[frame_len] = msg_body;

        if(frame_len >= 1 && incoming_frame[frame_len - 1] != '\x10') {
            switch (msg_body) {
                // Unescaped 0x03 found, signalling the end of the frame
                case '\x03':
                    // Send the complete frame
                    PassFrame(incoming_frame, frame_len + 1);
                    // Reset frame
                    frame_len = 0;
                    break;
                // Unescaped 0x02 found (That wasn't the first char), must restart 
                // the frame with this first character
                case '\x02':
                    #if DEBUGGING_TRAIN == 1
                    std::cout << "    PhysicalLayer::UARTMailboxLoop(): WARNING: Unescaped 0x02 found: >>";
                    for (int i = 0; i <= frame_len; i++) {
                        std::cout << HEX(incoming_frame[i]);
                    }
                    std::cout << "<<\n";
                    #endif

                    // Perform reset
                    incoming_frame[0] = msg_body;
                    frame_len = 1;
                    break;

                // They can escape any character, doesn't *have* to be a control character technically
                // case '\x10':
                //     std::cout << "PhysicalLayer::UARTMailboxLoop(): ERROR: UNESCAPED 0x10 FOUND\n";
                //     while (1) {}
                default:
                    frame_len++;
                    // A normal character, which was not escaped (This is fine)
                    break;
            }
        }
        else frame_len++;
    }
}

/*
    Function: PassFrame
    Brief: Passes frame up to the Data Link Layer, checking the checksum and 
            removing any escape characters
*/
void PhysicalLayer::PassFrame(unsigned char* frame_ptr, uint8_t frame_len) {
    static uint8_t msg_body[SMALL_LETTER];

    int msg_idx = 0;

    #if DEBUGGING_TRAIN == 1
    std::cout << "    PhysicalLayer::PassFrame: Frame from UART1, passing to DLL >>";
    for (int i = 0; i < frame_len; i++) {
        std::cout << HEX(frame_ptr[i]);
    }
    std::cout << "<<\n";
    #endif

    // Ensure frame starts with STX (0x02) and ends with ETX (0x03)
    // Note: These checks should be taken care of in the previous function. TODO: Remove
    assert(frame_len >= 5); // NACKs and ACKS are at least length 4 (Actually 5), DATA should be 6. More with escape characters
    assert(frame_ptr[0]             == '\x02');
    assert(frame_ptr[frame_len - 1] == '\x03');

    // Checks checksum
    uint8_t frame_checksum = 0;

    // TODO: This would be simpler if the loop just kept iterating when an escape character
    //       was found, leaving the bounds to be the bounds

    // Create msg_body to send up
    for(int i = 1; i < frame_len - 2; i++) {
        // If not an escape character ('0x10'), add the char to the msg_body
        if (frame_ptr[i] != '\x10') {
            // For debugging, ensure there are no unescaped 0x02 or 0x03 characters (Should be handled previously)
            assert(frame_ptr[i] != '\x02' && frame_ptr[i] != '\x03');
            msg_body[msg_idx] = uint8_t(frame_ptr[i]);
        }
        // If it is an escape character, add the character it is escaping (And ensure it is not escaping the checksum)
        else if (i + 1 != frame_len - 2) { // (i + 1) is the escaped character and (frame_len - 2) is the checksum
            // For debugging
            msg_body[msg_idx] = frame_ptr[++i]; // iterate i to add value of escaped character
        }
        frame_checksum += int(msg_body[msg_idx++]); // Add checksum value
    }

    frame_checksum = ONE_BYTE_MAX - frame_checksum;

    // If checksum passes, pass msg, otherwise drop it
    if (frame_checksum == frame_ptr[frame_len - 2]) {
        #if DEBUGGING_TRAIN == 1
        std::cout << "    PhysicalLayer::PassFrame(): Passed Checksum, passing up!\n";
        #endif

        #if DEBUGGING_TRAIN >= 1
        if (!PSend(UART_PHYSICAL_LAYER_MB, MONITOR_MB, (void *)msg_body, msg_idx)) {
            std::cout << "    PhysicalLayer::PassFrame(): WARNING -> Packet failed to send\n";
        }
        #endif

        // Send message up to Data Link Layer, without the STX, Checksum, or ETX
        if (!PSend(UART_PHYSICAL_LAYER_MB, DATA_LINK_LAYER_MB, (void *)msg_body, msg_idx)) {
            std::cout << "    PhysicalLayer::PassFrame(): WARNING -> Packet failed to send 2\n";
        }
    }
    // Not an error state, but unlikely enough to warrant a warning to user
    else {
        std::cout << "    PhysicalLayer::PassFrame(): WARNING -> Invalid Checksum, dropping frame >>" << HEX(frame_checksum) << "<< >>" << HEX(frame_ptr[frame_len - 2]) << "<<\n";
    }
}

void PhysicalLayer::PacketMailboxLoop() {
    // Bind PhysicalLayer queue
    if (!PBind(PACKET_PHYSICAL_LAYER_MB, SMALL_LETTER)) { // Default mailbox size of 16
        std::cout << "PhysicalLayer::PacketMailboxLoop(): WARNING Mailbox failed to bind!\n";
    }
    else std::cout << "PhysicalLayer::PacketMailboxLoop(): Mailbox bound\n";

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[SMALL_LETTER];
    static uint8_t frame_len = 0;

    while (1) {
        // Blocking message request
        PRecv(src_q, PACKET_PHYSICAL_LAYER_MB, &msg_body, mailbox_msg_len);
        
        // Error state checking for testing
        assert(mailbox_msg_len < SMALL_LETTER);
        assert(src_q == DATA_LINK_LAYER_MB); // Frame should always be coming from DLL

        #if DEBUGGING_TRAIN == 1
        std::cout << "    PhysicalLayer::PacketMailboxLoop: Frame from DLL, passing to UART1 >>";
        for (int i = 0; i < mailbox_msg_len; i++) {
            std::cout << HEX(msg_body[i]);
        }
        std::cout << "<<\n";
        #endif

        #if DEBUGGING_TRAIN == 1
        std::cout << "    PhysicalLayer::PacketMailboxLoop(): Packet from Data Link Layer, sending out UART1\n";
        #endif

        // Output Packet
            // This function handles adding the control characters, any necessary
            // escape characters, and the checksum
        ISRMsgHandlerInstance_->QueueOutputPacket(msg_body, mailbox_msg_len);
    }
}

void PhysicalLayer::FakePacket() {
    static unsigned char fake_frame[16];
    static bool first_time = true;
    static control_t control_block;
    static uint8_t frame_length = 10;

    static uint8_t prev_total = 198;

    // std::cout << "PhysicalLayer::FakePacket(): FAKING A PACKET!\n";

    // Set up fields that don't change
    if(first_time) {
        fake_frame[0] = '\x02'; // STX

        control_block.type = DATA_PT;
        control_block.nr = 0;
        control_block.ns = 0;

        // Always escape control block, just because
        fake_frame[1] = '\x10';

        fake_frame[2] = control_block.all;

        fake_frame[3] = '\x10'; // Escape the length
        fake_frame[4] = '\x03'; // Length of 2

        fake_frame[5] = '\xC2'; // CMD
        fake_frame[6] = '\x01'; // Train #1
        fake_frame[7] = '\x00'; // Sucess

        fake_frame[8] = '\x39'; // Checksum (255 - (3 + 194 + 1)) = 57 -> 0x39

        fake_frame[9] = '\x03'; // ETX

        first_time = false;
    }
    else {
        // Update control block
        MOD8PLUS1(control_block.ns);
        fake_frame[2] = control_block.all;

        // Update checksum
        fake_frame[8] = (255 - (prev_total + control_block.all));
    }

    // Send frame
    PassFrame(fake_frame, frame_length);
}

/*
    Function: PhysicalLayer
    Brief: Constructor for the PhysicalLayer class
*/
PhysicalLayer::PhysicalLayer() {
}

/*
    Function: ~PhysicalLayer
    Brief: Destructor for the PhysicalLayer class
*/
PhysicalLayer::~PhysicalLayer() {
}

/*
    Function: GetPhysicalLayer
    Output: PhysicalLayerInstance_: Pointer to the PhysicalLayer Singleton
    Brief: Get function for the PhysicalLayer singleton
*/
PhysicalLayer* PhysicalLayer::GetPhysicalLayer() {
    if (!PhysicalLayerInstance_) PhysicalLayerInstance_ = new PhysicalLayer;
    return PhysicalLayerInstance_;
}
