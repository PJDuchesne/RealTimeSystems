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

// Singleton Instance
PhysicalLayer *PhysicalLayer::PhysicalLayerInstance_ = 0;

/*
    Function: UARTMailboxLoop
    Brief: The main loop for checking data coming from the UART driver, byte by byte
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

    // Loop infinitly, sleeping between messages
    while (1) {
        // Blocking message request
        PRecv(src_q, UART_PHYSICAL_LAYER_MB, &msg_body, mailbox_msg_len);

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
                    // Perform reset
                    incoming_frame[0] = msg_body;
                    frame_len = 1;
                    break;

                // The default behavior is a non-escaped non-control character
                default:
                    frame_len++;
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

    // Ensure frame starts with STX (0x02) and ends with ETX (0x03)
    // Note: These checks should be taken care of in the previous function.
    assert(frame_len >= 4); // NACKs and ACKS are at length 5, DATA should be 6. More with escape characters
    assert(frame_ptr[0]             == '\x02');
    assert(frame_ptr[frame_len - 1] == '\x03');

    // Checks checksum
    uint8_t frame_checksum = 0;

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
            msg_body[msg_idx] = frame_ptr[++i]; // iterate i to add value of escaped character
        }
        frame_checksum += int(msg_body[msg_idx++]); // Add checksum value
    }

    frame_checksum = ONE_BYTE_MAX - frame_checksum;

    // If checksum passes, pass msg, otherwise drop it
    if (frame_checksum == frame_ptr[frame_len - 2]) {
        // Send message up to Data Link Layer, without the STX, Checksum, or ETX
        if (!PSend(UART_PHYSICAL_LAYER_MB, DATA_LINK_LAYER_MB, (void *)msg_body, msg_idx)) {
            std::cout << "    PhysicalLayer::PassFrame(): WARNING -> Packet failed to send to DLL\n";
        }
    }
    // Not an error state, but unlikely enough to warrant a warning
    else {
        std::cout << "    PhysicalLayer::PassFrame(): WARNING -> Invalid Checksum, dropping frame >>" << HEX(frame_checksum) << "<< >>" << HEX(frame_ptr[frame_len - 2]) << "<<\n";
    }
}

/*
    Function: PacketMailboxLoop
    Brief: This loop handles data coming down from the DLL looping to be passed out through the UART
    Note: This loop is handled by a separate process than the previous loop
*/
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

        // Output Packet
            // This function handles adding the control characters, any necessary
            // escape characters, and the checksum
        ISRMsgHandlerInstance_->QueueOutputPacket(msg_body, mailbox_msg_len);
    }
}

/*
    Function: FakePacket
    Brief: This function fakes a packet coming up from the UART and was used
        early on to test the DLL, this is obviously not used in the final code
*/
void PhysicalLayer::FakePacket() {
    static unsigned char fake_frame[16];
    static bool first_time = true;
    static control_t control_block;
    static uint8_t frame_length = 10;

    static uint8_t prev_total = 198;

    // Set up fields that don't change
    if(first_time) {
        fake_frame[0] = '\x02'; // STX

        control_block.type = DATA_PT;
        control_block.nr = 0;
        control_block.ns = 0;

        // Always escape control block, just in case
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
    Function: GetPhysicalLayer
    Output: PhysicalLayerInstance_: Pointer to the PhysicalLayer Singleton
    Brief: Get function for the PhysicalLayer singleton
*/
PhysicalLayer* PhysicalLayer::GetPhysicalLayer() {
    if (!PhysicalLayerInstance_) PhysicalLayerInstance_ = new PhysicalLayer;
    return PhysicalLayerInstance_;
}
