import sys
import usb.core
import usb.util

VENDOR_ID = 0x16C0
PRODUCT_ID = 0x05E1

dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)

while(dev == None):
    dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
    
#dev.set_configuration()


cfg = dev.get_active_configuration()
intf = cfg[(0,0)]

ep = usb.util.find_descriptor(
    intf,
    # match the first OUT endpoint
    custom_match = \
    lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_OUT)

assert ep is not None

ep.write("test")