
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sos/link/link_transport.h"

#include "mcu/core.h"
#include "mcu/debug.h"


#define pkt_checksum(pktp) ((pktp)->data[(pktp)->size])

static int send_ack(link_transport_driver_t * driver, uint8_t ack, uint8_t checksum);

int link_transport_slaveread(link_transport_driver_t * driver, void * buf, int nbyte, int (*callback)(void*,void*,int), void * context){
	link_pkt_t pkt;
	char * p;
	int bytes;
	uint8_t checksum;
	int err;

	bytes = 0;
	p = buf;
	do {

		if( (err = link_transport_wait_start(driver, &pkt, driver->timeout)) < 0 ){
			send_ack(driver, LINK_PACKET_NACK, 0);
			driver->wait(driver->timeout);
			driver->flush(driver->handle);
			return -1;
		}

		if( (err = link_transport_wait_packet(driver, &pkt, driver->timeout)) < 0 ){
			send_ack(driver, LINK_PACKET_NACK, 0);
			driver->wait(driver->timeout);
			driver->flush(driver->handle);
			return -2;
		}

		if( pkt.start != LINK_PACKET_START ){
			//if packet does not start with the start byte then it is not a packet
			send_ack(driver, LINK_PACKET_NACK, 0);
			driver->wait(driver->timeout*5);
			driver->flush(driver->handle);
			return -3;
		}

		checksum = pkt_checksum(&pkt);
		//a packet has arrived -- checksum it
		if( link_transport_checksum_isok(&pkt) == false ){
			//bad checksum on packet -- treat as a non-packet
			send_ack(driver, LINK_PACKET_NACK, checksum);
			driver->wait(driver->timeout);
			driver->flush(driver->handle);
			return -4;
		}

		//callback to handle incoming data as it arrives
		if( callback == NULL ){
			//copy the valid data to the buffer
			memcpy(p, pkt.data, pkt.size);
			bytes += pkt.size;
			p += pkt.size;
			send_ack(driver, LINK_PACKET_ACK, checksum);
		} else {
			if( callback(context, pkt.data, pkt.size) < 0 ){
				send_ack(driver, LINK_PACKET_NACK, checksum);
				return -5;
			} else {
				bytes += pkt.size;
				if( send_ack(driver, LINK_PACKET_ACK, checksum) < 0 ){
					return -6;
				}
			}
		}

	} while( (bytes < nbyte) && (pkt.size == LINK_PACKET_DATA_SIZE) );

	if( bytes == 0 ){
		driver->flush(driver->handle);
	}

	return bytes;
}

int link_transport_slavewrite(link_transport_driver_t * driver, const void * buf, int nbyte, int (*callback)(void*,void*,int), void * context){
	link_pkt_t pkt;
	char * p;
	int bytes;
	int ret;

	bytes = 0;
	p = (void*)buf;
	pkt.start = LINK_PACKET_START;
	do {

		if( (nbyte - bytes) > LINK_PACKET_DATA_SIZE ){
			pkt.size = LINK_PACKET_DATA_SIZE;
		} else {
			pkt.size = nbyte - bytes;
		}

		if( callback != NULL ){
			if( (ret = callback(context, pkt.data, pkt.size)) < 0 ){
				//could not get the desired data
				pkt.size = 0;
			} else {
				pkt.size = ret;
			}
		} else {
			//copy data from buf
			memcpy(pkt.data, p, pkt.size);
		}
		link_transport_insert_checksum(&pkt);

		//send packet
		if( driver->write(driver->handle, &pkt, pkt.size + LINK_PACKET_HEADER_SIZE) != (pkt.size + LINK_PACKET_HEADER_SIZE) ){
			return -1;
		}

		bytes += pkt.size;
		p += pkt.size;

	} while( (bytes < nbyte) && (pkt.size == LINK_PACKET_DATA_SIZE) );



	return bytes;
}

int send_ack(link_transport_driver_t * driver, uint8_t ack, uint8_t checksum){
	link_ack_t ack_pkt;
	ack_pkt.ack = ack;
	ack_pkt.checksum = checksum;
	return driver->write(driver->handle, &ack_pkt, sizeof(ack_pkt));
}

