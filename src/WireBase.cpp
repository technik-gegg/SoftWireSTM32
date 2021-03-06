/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file WireBase.cpp
 * @author Trystan Jones <crenn6977@gmail.com>
 * @brief Wire library, following the majority of the interface from Arduino.
 *        Provides a 'standard' interface to I2C (two-wire) communication for
 *        derived classes.
 */

/*
 * Library created by crenn to allow a system which would provide users the
 * 'standardised' Arduino method for interfacing with I2C devices regardless of
 * whether it is I2C hardware or emulating software.
 */

/*
 * Updated by Roger Clark. 20141111. Fixed issue when process() returned because of missing ACK (often caused by invalid device address being used), caused SCL to be left
 * LOW so that in the next call to process() , the first clock pulse was not sent, because SCL was LOW when it should have been high.
 */

/*
 * Updated by Brandon Green. 20172306. Implementing the repeated start functionality.
 */

/*
 * Library ported to Arduino Core STM32 2022-04-14 Technik Gegg
 */

#include "WireBase.h"

void WireBase::begin(uint8_t self_addr) {
    tx_buf_idx = 0;
    tx_buf_overflow = false;
    rx_buf_idx = 0;
    rx_buf_len = 0;
}

void WireBase::beginTransmission(uint8_t slave_address) {
    itc_msg.addr = slave_address;
    itc_msg.data = &tx_buf[tx_buf_idx];
    itc_msg.length = 0;
    itc_msg.flags = 0;
}

void WireBase::beginTransmission(int slave_address) {
    beginTransmission((uint8_t)slave_address);
}

uint8_t WireBase::endTransmission(void) {
    if (tx_buf_overflow) {
        return I2C_DATA_TOO_LONG;
    }
    uint8_t stat = process(); // added 2022-06-05 Technik Gegg
    tx_buf_idx = 0;
    tx_buf_overflow = false;
    return stat; 	// added 2022-06-05 Technik Gegg
					// returning I2C_OK doesn't reflect the current status (i.e. I2C_NACK_ADDR)
					// and hence a bus scan will always deliver a found device at the
					// given address!
}

//TODO: Add the ability to queue messages (adding a bool to end of function
// call, allows for the Arduino style to stay while also giving the flexibility
// to bulk send
uint8_t WireBase::requestFrom(uint8_t address, int num_bytes) {
    if (num_bytes > I2C_TXRX_BUFFER_SIZE) {
        num_bytes = I2C_TXRX_BUFFER_SIZE;
    }
    itc_msg.addr = address;
    itc_msg.flags = I2C_MSG_READ;
    itc_msg.length = num_bytes;
    itc_msg.data = &rx_buf[rx_buf_idx];
    process();
    rx_buf_len += itc_msg.xferred;
    itc_msg.flags = 0;
    return rx_buf_len;
}

uint8_t WireBase::requestFrom(int address, int numBytes) {
    return WireBase::requestFrom((uint8_t)address, numBytes);
}

void WireBase::write(uint8_t value) {
    if (tx_buf_idx == I2C_TXRX_BUFFER_SIZE) {
        tx_buf_overflow = true;
        return;
    }
    tx_buf[tx_buf_idx++] = value;
    itc_msg.length++;
}

void WireBase::write(uint8_t* buf, int len) {
    for (uint8_t i = 0; i < len; i++) {
        write(buf[i]);
    }
}

void WireBase::write(int value) {
    write((uint8_t)value);
}

void WireBase::write(int* buf, int len) {
    write((uint8_t*)buf, (uint8_t)len);
}

void WireBase::write(char* buf) {
    uint8_t *ptr = (uint8_t*)buf;
    while (*ptr) {
        write(*ptr);
        ptr++;
    }
}

uint8_t WireBase::available() {
    return rx_buf_len - rx_buf_idx;
}

uint8_t WireBase::read() {
    if (rx_buf_idx == rx_buf_len) {
        rx_buf_idx = 0;
        rx_buf_len = 0;
        return 0;
    } else if (rx_buf_idx == (rx_buf_len-1)) {
        uint8_t temp = rx_buf[rx_buf_idx];
        rx_buf_idx = 0;
        rx_buf_len = 0;
        return temp;
    }
    return rx_buf[rx_buf_idx++];
}
