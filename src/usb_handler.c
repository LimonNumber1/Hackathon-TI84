#define _DEBUG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keypadc.h>

#include <usbdrvce.h>
#include <srldrvce.h>

#include "usb_handler.h"
#include "helper.h"

/* Serial device struct */
srl_device_t srl;

bool has_device = false;


/* A buffer for internal use by the serial library */
uint8_t srl_buf[512];

/* Handle USB events */
static usb_error_t handle_usb_event(usb_event_t event, void *event_data,
                                    usb_callback_data_t *callback_data) {
    // to not get the warning of unused var
    
    /* Enable newly connected devices */
    if(event == USB_DEVICE_CONNECTED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE)) {
        usb_device_t device = event_data;
        os_PutStrFull("ln 38");
        usb_ResetDevice(device);
    }
    /* When a device is connected, or when connected to a computer */
    if((event == USB_DEVICE_ENABLED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE)) || event == USB_HOST_CONFIGURE_EVENT) {
        usb_device_t device;
        os_PutStrFull("ln 43");
        if(event == USB_HOST_CONFIGURE_EVENT) {
            device = usb_FindDevice(NULL, NULL, USB_SKIP_HUBS);
        } else {
            device = event_data;
        }

        if(device && !has_device) {
            /* Initialize the serial library with the newly attached device */
            os_PutStrFull("ln 53");
            srl_error_t error = srl_Open(&srl, device, srl_buf, sizeof srl_buf, SRL_INTERFACE_ANY);

            if(error) {
                /* Print the error code to the homescreen */
                char buf[64];
                sprintf(buf, "Error %u initting serial", error);

                os_ClrHome();
                debug_log(buf);
            } else {
                os_PutStrFull("ln 63");
                has_device = true;
            }
        }
    }
    if(event == USB_DEVICE_DISCONNECTED_EVENT) {
        os_PutStrFull("ln 70");
        has_device = false;
    }

    return USB_SUCCESS;
}

int network_init() {
    os_PutStrLine ("Initializing usb connection...\n"); 
    /* Initialize the USB driver with our event handler and the serial device descriptors */

    usb_error_t usb_error = usb_Init(handle_usb_event, NULL, srl_GetCDCStandardDescriptors(), USB_DEFAULT_INIT_FLAGS);
    if(usb_error) {
        os_PutStrFull("ERROR: could not initialize usb connection (usb_handler.c, 80)");
        usb_Cleanup();
        exit(1);
    }
}

void put_char(const char c) {
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';

    os_PutStrFull(buf);
}

void aseae(void* data, int sz) {
    srl_Read_Blocking(&srl, data, sz, 10000);
}

bool read_packet(struct packet_t* p) {
    size_t sz = srl_Read_Blocking(&srl, p, sizeof(struct packet_t), 1000);
    if (sz != sizeof(struct packet_t))
        return false;
    return true;
    
}

bool send_packet(struct packet_t* p) {
    size_t sz = srl_Write_Blocking(&srl, p, sizeof(struct packet_t), 1000);
    if (sz != sizeof(struct packet_t))
        return false;
    return true;
}