#include "stm32f0xx.h"
#include "stdint.h"


class Packet {
    public:

        /*
        * HEADER VALUES
        * length will be truncated for use, but storing these variables
        * in uints since there arent some wacky types like uint5_t or 
        * uint30_t.
        */

        // [long, msg] 7-bit quantity determining length of data stream
        // [short] 3 bit quantity
        unsigned int length;

        // [long] 4 bit
        unsigned int first_be;

        // [long] 4 bit
        unsigned int last_be;

        //[long, short, msg] 5-bit destination address
        unsigned int destination;

        //[long, short, msg] 5-big requester ID
        unsigned int requester;
        
        //[long, short, msg] 4-bit format code
        // long memory read   - 0x0
        // long memory write  - 0x1
        // message            - 0x2        
        // short memory read  - 0x8
        // short memory write - 0x9
        unsigned int format;

        //[msg] 10-bit message code.
        // TODO: copy defined codes from drawio
        uint32_t msg_code;

        // [long] 30-bit address
        // [short] 16-bit address
        uint32_t address;

        // word pointer to the data. when packets are constructed,
        // only *data to *(data + length) will be sent.
        // since data is a pointer, should be DMA-compatable (might want to check this)
        uint32_t* data;

        uint32_t* packet_bitstream;


        /*
        * @brief Sets a uint32_t (word) array of packet data using this packet object's header properties and data.
        *        Sets the array to the packet_construct field in this packet object.
        */
        void construct_packet();

        // TODO: this
        void deconstruct_packet();


        /*
        * Empty packet constructor. Doesn't assign anything
        */
        Packet();

        /*
        * @brief Packet constructor for the LONG format
        * @param length_ length of DATA, not packet.
        * @param first_be_ first byte enable
        * @param last_be_ last byte enable
        * @param dest_ destination address
        * @param requester_ requester ID
        * @param format_ format code (see format field for those values)
        * @param addr_ address to send / recieve data to/from.
        * @param data_ pointer to the packet's data.
        */
        Packet(unsigned int length_, unsigned int first_be_, unsigned int last_be_, unsigned int dest_, unsigned int requester_, unsigned int format_, uint32_t addr_, uint32_t* data_);

        /*
        * @brief Packet object constructor for SHORT AND MSG format.
        * @param length_ length of DATA
        * @param addr_ address to send/rec data to/from, ALSO USED AS msg_code for MESSAGE type packets
        * @param dest_ destination address
        * @param requester_ requester ID
        * @param format_ format code (see format field for those values)
        * @param data_ pointer to packet data
        */
        Packet(unsigned int length_, uint32_t addr_, unsigned int dest_, unsigned int requester_, unsigned int format_, uint32_t* data_);


        /*
        * @brief Creates a packet object from a packet bitstream, determines format and length and all that jazz
        * @param packet_bitstream_ array of uint32_t values that represent the packet's bitstream, flit by flit.
        * 
        */
        Packet(uint32_t* packet_bitstream_);

        // TODO: packet constructor without length, where it figures out length from data_

        /*
        * @brief Packet object destructor, deletes packet_construct so no memory leak
        */
        ~Packet();

        
};