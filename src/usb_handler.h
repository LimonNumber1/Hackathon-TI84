#ifndef USB_HANDLER_H
#define USB_HANDLER_H

#ifndef BYTE
#define BYTE
typedef unsigned char byte;
#endif

union packet_data {
    char msg[1000];
    char name[10];
};

struct packet_t {
    int command;
    union packet_data pd;
};


enum commands {
    command_init = 1,
    command_connect,
    command_disconnect,
    command_sendmsg,
    command_name
};

bool read_packet(struct packet_t* p);

bool send_packet(struct packet_t* p);

int network_init();

#endif // USB_HANDLER_H