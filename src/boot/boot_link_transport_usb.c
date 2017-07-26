

#include <errno.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "cortexm/cortexm.h"
#include "sos/link.h"
#include "mcu/mcu.h"
#include "sos/dev/usb.h"
#include "device/usbfifo.h"
#include "usbd/control.h"
#include "usbd/cdc.h"
#include "cortexm/cortexm.h"
#include "mcu/core.h"
#include "mcu/usb.h"
#include "mcu/pio.h"
#include "mcu/debug.h"
#include "mcu/boot_debug.h"

#include "sos/link/transport_usb.h"

#define USBDEV_PORT 0

const devfs_handle_t usb_dev = { .port = USBDEV_PORT };

#define BUF_SIZE 256
static char rd_buffer[BUF_SIZE];
static int rd_tail;
static int rd_head;

link_transport_phy_t boot_link_transport_usb_open(const char * name,
		usbd_control_t * context,
		const usbd_control_constants_t * constants,
		const usb_attr_t * usb_attr,
		mcu_pin_t usb_up_pin,
		int usb_up_active_high){

	pio_attr_t pio_attr;
	devfs_handle_t pio_handle;
	pio_handle.port = usb_up_pin.port;

	dstr("CONFIGURE UP PIN\n");


	pio_attr.o_pinmask = (1<<usb_up_pin.pin);

	if( usb_up_active_high ){
		mcu_pio_clrmask(&pio_handle, (void*)(pio_attr.o_pinmask));
	} else {
		mcu_pio_setmask(&pio_handle, (void*)(pio_attr.o_pinmask));
	}

	mcu_pio_setmask(&pio_handle, (void*)(pio_attr.o_pinmask));
	pio_attr.o_flags = PIO_FLAG_SET_OUTPUT | PIO_FLAG_IS_DIRONLY;
	mcu_pio_setattr(&pio_handle, &pio_attr);

	memset(context, 0, sizeof(usbd_control_t));
	context->constants = constants;
	context->handle = &(constants->handle);

	dstr("OPEN USB\n");
	//open USB
	cortexm_delay_ms(250);
	mcu_usb_open(&usb_dev);


	dstr("SET USB ATTR\n");
	if( mcu_usb_setattr(&usb_dev, (void*)usb_attr) < 0 ){
		dstr("FAILED TO SET USB ATTR\n");
	}

	dstr("USB ATTR SET\n");
	//initialize USB device
	usbd_control_priv_init(context);
	dstr("USB CONNECT\n");


	rd_tail = 0;
	rd_head = 0;

	if( usb_up_active_high ){
		mcu_pio_setmask(&pio_handle, (void*)(pio_attr.o_pinmask));
	} else {
		mcu_pio_clrmask(&pio_handle, (void*)(pio_attr.o_pinmask));
	}

	return 0;
}

int boot_link_transport_usb_write(link_transport_phy_t handle, const void * buf, int nbyte){
	int ret;
	ret = mcu_sync_io(&usb_dev, mcu_usb_write, SOS_LINK_TRANSPORT_USB_BULK_ENDPOINT_IN, buf, nbyte, O_RDWR);
	return ret;
}

static int read_buffer(char * dest, int nbyte){
	int i;
	for(i=0; i < nbyte; i++){
		if ( rd_head == rd_tail ){ //check for data in the fifo buffer
			//there is no more data in the buffer
			break;
		} else {
			dest[i] = rd_buffer[rd_tail];
			rd_tail++;
			if ( rd_tail == BUF_SIZE){
				rd_tail = 0;
			}
		}
	}
	return i; //number of bytes read
}

static int write_buffer(const char * src, int nbyte){
	int i;
	for(i=0; i < nbyte; i++){
		if( ((rd_head+1) == rd_tail) ||
				((rd_tail == 0) && (rd_head == (BUF_SIZE-1))) ){
			break; //no more room
		}

		rd_buffer[rd_head] = src[i];
		rd_head++;
		if ( rd_head == BUF_SIZE ){
			rd_head = 0;
		}
	}
	return i; //number of bytes written
}

int boot_link_transport_usb_read(link_transport_phy_t handle, void * buf, int nbyte){
	int ret;
	int bytes_read;
	char buffer[SOS_LINK_TRANSPORT_ENDPOINT_SIZE];
	bytes_read = 0;
	ret = read_buffer(buf, nbyte);
	bytes_read += ret;

	while( bytes_read < nbyte ){
		//need more data to service request
		ret = mcu_sync_io(&usb_dev, mcu_usb_read, SOS_LINK_TRANSPORT_USB_BULK_ENDPOINT_OUT, buffer, SOS_LINK_TRANSPORT_ENDPOINT_SIZE, O_RDWR | MCU_SYNC_IO_FLAG_READ);
		write_buffer(buffer, ret);
		ret = read_buffer(buf + bytes_read, nbyte - bytes_read);
		bytes_read += ret;
	}
	return nbyte;
}

int boot_link_transport_usb_close(link_transport_phy_t * handle){
	return mcu_usb_close(&usb_dev);
}

void boot_link_transport_usb_wait(int msec){
	int i;
	for(i = 0; i < msec; i++){
		cortexm_delay_us(1000);
	}
}

void boot_link_transport_usb_flush(link_transport_phy_t handle){}

