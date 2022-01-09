#ifndef USB_HANDLER_H
#define USB_HANDLER_H

#ifndef BYTE
#define BYTE
typedef unsigned char byte;
#endif

union packet_data {
    bool ready;
    byte  position[2];
    bool isHit;
    bool isSunk;
};

struct packet_t {
    int command;
    union packet_data pd;
};


enum commands {
    command_init,
    command_start,
    command_stop,
    command_attack,
    command_respond
};

bool read_packet(struct packet_t* p);

bool send_packet(struct packet_t* p);

int network_init();

#endif // USB_HANDLER_H