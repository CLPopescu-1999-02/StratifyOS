/* Copyright 2011-2018 Tyler Gilbert;
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

/*! \addtogroup SWITCHBOARD Switchboard
 * @{
 *
 * \ingroup IFACE_DEV
 *
 * \details The switchboard device driver
 * is a device that is used to connect
 * other devices and transfer data in
 * the background.
 *
 * For example, the switchboard can read data
 * from a UART and write it to a FIFO. It can
 * also implement UART loopback functionality.
 *
 * It is also well-suited for streaming audio
 * from one interface to another (say from
 * I2S input to a USB audio class endpoint.
 *
 * The switchboard is configured using ioctl calls.
 *
 * Reading the switchboard will read a block device
 * populated with the status of the connections.
 *
 * \code
 *
 * switchboard_status_t status;
 *
 * int fd = open("/dev/switchboard", O_RDWR);
 *
 * while( read(&status, sizeof(status)) > 0 ){
 *   printf("Connection is active:%d\n", status.o_flags != 0 );
 * }
 *
 * \endcode
 *
 */

/*! \file
 * \brief Switchboard Header File
 *
 */

#ifndef DEV_SOS_SWITCHBOARD_H
#define DEV_SOS_SWITCHBOARD_H

#include "sos/fs/devfs.h"
#include "sos/link.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWITCHBOARD_VERSION (0x030600)
#define SWITCHBOARD_IOC_IDENT_CHAR 'W'

typedef enum {
    SWITCHBOARD_FLAG_CONNECT /*! Configure a switchboard connection (used in o_flags of switchboard_attr_t) */ = (1<<0),
    SWITCHBOARD_FLAG_DISCONNECT /*! Clears the specified connection (used in o_flags of switchboard_attr_t) */ = (1<<1),
    SWITCHBOARD_FLAG_IS_PERSISTENT /*! Set to configure a persistent connection (used in o_flags of switchboard_attr_t) */ = (1<<2),
    SWITCHBOARD_FLAG_IS_CONNECTED /*! Used in o_flags of switchboard_connection_t for status */ = (1<<3),
    SWITCHBOARD_FLAG_IS_READING_ASYNC /*! Used in o_flags of switchboard_connection_t for status */ = (1<<4),
    SWITCHBOARD_FLAG_IS_WRITING_ASYNC /*! Used in o_flags of switchboard_connection_t for status */ = (1<<5),
    SWITCHBOARD_FLAG_IS_PRIMARY_BUFFER_USED /*! Used in o_flags of switchboard_connection_t for status */ = (1<<6),
    SWITCHBOARD_FLAG_IS_SECONDARY_BUFFER_USED /*! Used in o_flags of switchboard_connection_t for status */ = (1<<7),
    SWITCHBOARD_FLAG_SET_TRANSACTION_LIMIT /*! Use with SWITCHBOARD_FLAG_CONNECT to specify a transaction limit other than the default */ = (1<<8)
} switchboard_flag_t;


typedef struct MCU_PACK {
    u32 o_flags /*! Bitmask of supported flags */;
    u16 connection_count /*! The total number of connections available */;
    u16 connection_buffer_size /*! The internal buffer size (max number of bytes for persistent connections) */;
    u32 transaction_limit /*! The maximum number of synchronous transactions that are allowed before the connection is aborted */;
    u32 resd[8];
} switchboard_info_t;

/*! \brief Switchboard Terminal
 * \details Contains the data for
 * a switchboard terminal.
 *
 */
typedef struct MCU_PACK {
    char name[LINK_NAME_MAX] /*! The name of the terminal */;
    u32 loc /*! The location of the terminal (block location or channel depending on the device) */;
    u32 bytes_transferred /*! Number of bytes transferred on the terminal */;
    s8 priority /*! Hardware interrupt priority elevation */;
    s8 resd[3]; //alignment
} switchboard_terminal_t;


/*! \brief Switchboard Connection
 * \details A switchboard connection
 * defines input and output terminals
 * as well as the number of bytes to
 * transfer.
 *
 * \sa switchboard_status_t
 *
 */
typedef struct MCU_PACK {
    u16 idx /*! Connection index of total */;
    u16 o_flags /*! Bitmask flags for connection state */;
    switchboard_terminal_t input /*! Input device (device that is read) */;
    switchboard_terminal_t output /*! Output device (device that is written) */;
    s32 nbyte /*! Number of bytes to transfer (packet size for persisent connections); will be negative when reading to indicate an error */;
    u16 transaction_limit /*! The maximum number of synchronous transactions that can occur before aborting */;
} switchboard_connection_t;

/*!
 * \brief Switchboard Status
 * \details Data type that describes the data
 * that is read from the switchboard.
 *
 * \code
 * int fd = open("/dev/switchboard", O_RDWR);
 * switchboard_status_t status;
 * read(fd, &status, sizeof(status));
 *
 * read(fd, &status, 2); //returns an error with errno set to EINVAL
 * \endcode
 *
 */
typedef switchboard_connection_t switchboard_status_t;

/*! \brief Switchboard Attributes
 * \details Switchboard attributes are used with I_SWITCHBOARD_SETATTR
 * to connect and disconnect terminals.
 *
 */
typedef struct MCU_PACK {
    u32 o_flags /*! Flag bitmask such as SWITCHBOARD_FLAG_CONNECT */;
    switchboard_connection_t connection /*! The switchboard connection details. */;
    u32 resd[8];
} switchboard_attr_t;


#define I_SWITCHBOARD_GETVERSION _IOCTL(SWITCHBOARD_IOC_IDENT_CHAR, I_MCU_GETVERSION)

/*! \brief This request gets the SPI attributes.
 * \hideinitializer
 */
#define I_SWITCHBOARD_GETINFO _IOCTLR(SWITCHBOARD_IOC_IDENT_CHAR, I_MCU_GETINFO, switchboard_info_t)

/*! \brief See details below.
 * \hideinitializer
 *
 * \details Sets the switchboard attributes. This IOCTL
 * call is used for configuring connections on the switch board.
 *
 * \code
 * #include <sos/dev/switchboard.h>
 * #include <fcntl.h>
 * #include <unistd.h>
 *
 * int uart_fd = open("/dev/uart0", O_RDWR);
 * ioctl(uart_fd, I_UART_SETATTR); //set default attributes
 *
 * //now configure the UART in loopback mode
 * int fd = open("/dev/switchboard", O_RDWR);
 * switchboard_attr_t attr;
 * strcpy(attr.input_terminal, "uart0");
 * strcpy(attr.output_terminal, "uart0");
 * attr.o_flags = SWITCHBOARD_FLAG_CONNECT |
 *  SWITCHBOARD_FLAG_IS_PERSISTENT;
 * ioctl(fd, I_SWITCHBOARD_SETATTR, &attr); //uart0 is in loopback mode
 * close(fd); //connection persists until disconnected
 *
 * \endcode
 *
 */
#define I_SWITCHBOARD_SETATTR _IOCTLW(SWITCHBOARD_IOC_IDENT_CHAR, I_MCU_SETATTR, switchboard_attr_t)
#define I_SWITCHBOARD_SETACTION _IOCTLW(SWITCHBOARD_IOC_IDENT_CHAR, I_MCU_SETACTION, mcu_action_t)


#define I_SWITCHBOARD_TOTAL 1

#ifdef __cplusplus
}
#endif

#endif // DEV_SOS_SWITCHBOARD_H
