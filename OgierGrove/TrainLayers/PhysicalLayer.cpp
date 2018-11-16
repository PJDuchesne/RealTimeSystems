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
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/PhysicalLayer.h"

// Singleton Instance
PhysicalLayer *PhysicalLayer::PhysicalLayerInstance_ = 0;

/*
    Function: UARTMailboxLoop
    Brief: <>
*/
void PhysicalLayer::UARTMailboxLoop() {
    // Bind PhysicalLayer queue
    PBind(UART_PHYSICAL_LAYER_MB, ONE_CHAR, UART_PHYSICAL_LAYER_MB_SIZE);

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body;
    static unsigned char incoming_frame[MAX_FRAME_SIZE];
    static uint8_t frame_len = 0;

    while (1) {
        // Blocking message request
        PRecv(src_q, ISR_MSG_HANDLER_MB, &msg_body, mailbox_msg_len);
        
        // Error state checking for testing
        assert(mailbox_msg_len < 256);
        assert(frame_len < MAX_FRAME_SIZE);

        // Add msg to internal msg maker
        incoming_frame[frame_len] = msg_body;

        // Check if this is the end of the frame (Needs to be at least 2 length and a non-escaped ETX (0x03))
        if (frame_len >= 1 && incoming_frame[frame_len - 2] != '\x10' && incoming_frame[frame_len - 1] == '\x03') {
            // Handle Frame
            PassFrame(incoming_frame, frame_len);

            // Reset Length
            frame_len = 0;
        }
        else frame_len++;
    }
}

/*
    Function: PassFrame
    Brief: <>
*/
void PhysicalLayer::PassFrame(unsigned char* frame_ptr, uint8_t frame_len) {
    // Ensure frame starts with STX (0x02) and ends with ETX (0x03)
    assert(frame_ptr[0]         == '\x02');
    assert(frame_ptr[frame_len] == '\x03');

    // Checks checksum
    uint8_t frame_checksum = 0;

    for(int i = 1; i < frame_len - 2; i++) {
        if (frame_ptr[i] != '\x10') frame_checksum += int(frame_ptr[i]);
        else {
            // Check that the next character is not the checksum itself 
            assert(i + 1 < frame_len - 1);
            frame_checksum += frame_ptr[++i];
        }
    }

    // If checksum passes, pass msg, otherwise drop it
    if ((ONE_BYTE_MAX - frame_checksum) == frame_ptr[frame_len - 1]) {
        // Send message up to Data Link Layer, without the STX, Checksum, or ETX
        PSend(UART_PHYSICAL_LAYER_MB, DATA_LINK_LAYER_MB, (void *)frame_ptr[1], frame_len - 2);
    }
    // Not an error state, but unlikely enough to warrant a warning to user
    else std::cout << "PhysicalLayer::PassFrame(): WARNING -> DROPPING FRAME DUE TO INVALID CHECKSUM\n";
}

void PhysicalLayer::PacketMailboxLoop() {
    // Bind PhysicalLayer queue
    PBind(PACKET_PHYSICAL_LAYER_MB, ONE_CHAR); // Default mailbox size of 16

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[256];
    static uint8_t frame_len = 0;

    while (1) {
        // Blocking message request
        PRecv(src_q, PACKET_PHYSICAL_LAYER_MB, &msg_body, mailbox_msg_len);
        
        // Error state checking for testing
        assert(mailbox_msg_len < 255);
        assert(src_q == DATA_LINK_LAYER_MB); // Frame should always be coming from DLL

        // Output Packet
            // This function handles adding the control characters, any necessary
            // escape characters, and the checksum
        ISRMsgHandlerInstance_->QueueOutputPacket(msg_body, mailbox_msg_len);
    }
}

/*
    Function: PhysicalLayer
    Brief: Constructor for the PhysicalLayer class, which initializes the data_buffer_ on startup
*/
PhysicalLayer::PhysicalLayer() {
    uart1_output_data_buffer_ = new RingBuffer<char>(UART1_OUTPUT_DATA_BUFFER_SIZE);
}

/*
    Function: ~PhysicalLayer
    Brief: Destructor for the PhysicalLayer class, which deletes the data_buffer_ on shutdown
*/
PhysicalLayer::~PhysicalLayer() {
    delete uart1_output_data_buffer_;
}

void PhysicalLayer::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
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
