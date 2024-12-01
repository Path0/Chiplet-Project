#include "stm32f0xx.h"
#include "stdint.h"

#include "Packet.h"



void Packet::construct_packet() {
    // set the packet_construct pointer to an empty array of the length we want
    unsigned int header_length;
    
    // long format header is 2 words, msg and short are 1
    if((format == 0x0) || (format == 0x1)) header_length = 2; else header_length = 1;

    // allocate the memory for this packet
    packet_bitstream = new uint32_t[header_length + length];

    // initial 1 or 2 words (depending on format) before data words
    uint32_t word_0;
    uint32_t word_1;

    // long read/write format
    if((format == 0x0) || (format == 0x1)) { 
        word_0 = 
            ((length & 0x7F) << 0) |
            ((first_be & 0xF) << 7) |
            ((last_be & 0xF) << 11) |
            ((destination & 0x1F) << 18) |
            ((requester & 0x1F) << 23) |
            ((format & 0xF) << 28) ;

        word_1 = address << 2;

        *packet_bitstream = word_0;
        *(packet_bitstream + 1) = word_1;

        //if the encoding for length = 0x0, that means 128 words for long packets
        unsigned int loopTimes;
        if(length == 0x0) loopTimes = 128; else loopTimes = length;

        // fix if length was accidentally set higher, only transmit 128 words
        if(loopTimes > 128) loopTimes = 128;

        // fill the rest of the packet with the data
        for(unsigned int x = 0; x < loopTimes; x++) {  
            *(packet_bitstream + x + 2) = *(data + x);
        }

    // short read/write format    
    } else if((format == 0x8) || (format == 0x9)) {
        word_0 = 
            ((length & 0x7) << 0) |
            ((address & 0xFFFF) << 2) |
            ((destination & 0x1F) << 18) |
            ((requester & 0x1F) << 23) |
            ((format & 0xF) << 28) ;

        *packet_bitstream = word_0;

        //if the encoding for length is 0x0, that means 4 words for short packets
        unsigned int loopTimes;
        if(length == 0x0) loopTimes = 128; else loopTimes = length;
        
        // short packet format, fix any length greater than 4
        if(loopTimes > 4) loopTimes = 4; 
        // fill the rest of the packet with the data
        for(unsigned int x = 0; x < loopTimes; x++) { 
            *(packet_bitstream + x + 1) = *(data + x);
        }

    // message format
    } else if (format == 0x2) {
        word_0 = 
            ((length & 0x7) << 0) |
            ((msg_code & 0x7FF) << 7) |
            ((destination & 0x1F) << 18) |
            ((requester & 0x1F) << 23) |
            ((format & 0xF) << 28) ;

        *packet_bitstream = word_0;

        //length 0 possible here
        // fill the rest of the packet with the data, works if data=0
        for(unsigned int x = 0; x < length; x++) { 
            *(packet_bitstream + x + 1) = *(data + x);
        }
    // if wrong format, the packet is just one word of F's
    } else {
        *packet_bitstream = 0xFFFFFFFF;
    }

}


Packet::Packet(unsigned int length_, unsigned int first_be_, unsigned int last_be_, unsigned int dest_, unsigned int requester_, unsigned int format_, uint32_t addr_, uint32_t* data_) {
            // sets fields
            length = length_;
            first_be = first_be_;
            last_be = last_be_;
            destination = dest_;
            requester = requester_;
            format = format_;
            address = addr_;
            data = data_;
        }


Packet::Packet(unsigned int length_, uint32_t addr_, unsigned int dest_, unsigned int requester_, unsigned int format_, uint32_t* data_) {
            length = length_;
            if(format_ == 0x2) {
                msg_code = addr_;
            }
            address = addr_;
            destination = dest_;
            requester = requester_;
            format = format_;
            data = data_;
        }


Packet::Packet(uint32_t* packet_bitstream_) {
    // set bitstream to bitstream, for if we need to send this out for some reason
    packet_bitstream = packet_bitstream_;

    uint32_t word_0 = *packet_bitstream;

    // set the format to the last 4 bits of word_0, but bit shift them back so it's usable
    format = (word_0 & (0xF << 28)) >> 28;

    // do the same with requester and dest, since those are both in the same place no matter the format
    requester = (word_0 & (0x1F << 23)) >> 23;
    destination = (word_0 & (0x1F << 18)) >> 18;

    // depending on format, do different things

    // long read/write format
    if((format == 0x0) || (format == 0x1)) {

        // set the rest of the first word's header variablesxx
        last_be = (word_0 & (0xF << 11)) >> 11;
        first_be = (word_0 & (0xF << 7)) >> 7;
        length = (word_0 & (0x7F << 0)) >> 0;


        // for long format packets, get the address from the second word
        uint32_t word_1 = *(packet_bitstream + 1);
        address = word_1 >> 2;



    }

    // short read/write format
    else if((format == 0x8) || (format == 0x9)) {
        address = (word_0 & (0xFFFF << 2)) >> 2;
        length = (word_0 & (0x3 << 0)) >> 0;

    }

    // message format
    else if(format == 0x2) {

        msg_code = (word_0 & (0x7FF << 7)) >> 7;
        length = (word_0 & (0x7F << 0)) >> 0;



    }

    // if the format isn't valid, TODO: do something weird
    else {

    }

    

    // data time
    data = new uint32_t[length];

    // set the offset to 1 since we dont want to put non-data into the data array
    unsigned int offset = 1;
    if((format == 0x0) || (format == 0x1)) {
        offset++;
    }
    
    // loop through data, starting at index up to index+length
    for(unsigned int x = 0; x < length; x++) {
        // copy the data into the data array
        *(data + x) = *(packet_bitstream + offset + x);
    }


}

Packet::Packet() {};

Packet::~Packet() {
    delete packet_bitstream;
}