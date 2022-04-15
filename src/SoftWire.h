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
 * @file SoftWire.h
 * @author Trystan Jones <crenn6977@gmail.com>
 * @brief Wire library, uses the WireBase to create the primary interface
 *        while keeping low level interactions invisible to the user.
 */

/*
 * Library updated by crenn to follow new Wire system.
 * Code was derived from the original Wire for maple code by leaflabs and the
 * modifications by gke and ala42.
 */

/*
 * Updated by Roger Clark. 20141111. Fixed issue when process() returned because of missing ACK (often caused by invalid device address being used), caused SCL to be left
 * LOW so that in the next call to process() , the first clock pulse was not sent, because SCL was LOW when it should have been high.
 */

/*
 * Updated by Brandon Green. 20172306. Implementing the repeated start functionality.
 */

/*
 * Ported to Arduino Core STM32 2022-04-14 Technik Gegg
 */

#pragma once

#include <Arduino.h>
#include "WireBase.h"

#if defined(STM32_CORE_VERSION)
typedef uint32_t pin_t;
#else
typedef uint8_t pin_t;
#endif

// The following values have been proven working on the STM32F103.
// With the SOFT_FAST setting you can expect a speed of around
// 240 kHz, with SOFT_STANDARD around 90 kHz on a 72 MHz MCU.
// This is a feasible speed for driving I2C displays for example.
// For using this library with other (faster) MCU's, it's best practice 
// to overwrite the I2C_Delay function and set the delay as needed.
#define SOFT_STANDARD   3    
#define SOFT_FAST       1     

/**
 * @brief Weakened function for inserting delays after SDA/SCL have been set/reset.
 *        Since this function is declared as WEAK, one can easily overwrite it
 *        and hence setup a different/more precise timing, if needed.
 * 
 * @param loops   The number of iterations (loops). 
 *                Usually the number that's being set in i2c_delay member. 
 */
extern WEAK void I2C_Delay(int loops);

class SoftWire : public WireBase
{
private:
   uint8_t i2c_delay;
   PinName scl_pin;     // using PinName types allows the usage of digitalWriteFast() / digitalReadFast()
   PinName sda_pin;

   /*
    * Sets the SCL line to HIGH/LOW and allow for clock stretching by slave
    * devices
    */
   void set_scl(bool);

   /*
    * Sets the SDA line to HIGH/LOW
    */
   void set_sda(bool);

   /*
    * Creates a Start condition on the bus
    */
   void i2c_start();

   /*
    * Creates a Stop condition on the bus
    */
   void i2c_stop();

   /*
    * Created a Repeated Start condition on the bus
    */
   void i2c_repeated_start();

   /*
    * Gets an ACK condition from a slave device on the bus
    */
   bool i2c_get_ack();

   /*
    * Creates a ACK condition on the bus
    */
   void i2c_send_ack();

   /*
    * Creates a NACK condition on the bus
    */
   void i2c_send_nack();

   /*
    * Shifts in the data through SDA and clocks SCL for the slave device
    */
   uint8_t i2c_shift_in();

   /*
    * Shifts out the data through SDA and clocks SCL for the slave device
    */
   void i2c_shift_out(uint8_t);

protected:
   /*
    * Processes the incoming I2C message defined by WireBase
    */
   uint8_t process(uint8_t);
   uint8_t process();

public:
   /*
    * Accept pin numbers for SCL and SDA lines. Set the delay needed
    * to create the timing for I2C's Standard Mode and Fast Mode.
    */
   SoftWire(pin_t sda = SDA, pin_t scl = SCL, uint8_t delay = SOFT_STANDARD);

   /*
    * Sets pins SDA and SCL to OUPTUT_OPEN_DRAIN, joining I2C bus as
    * master. This function overwrites the default behaviour of
    * .begin(uint8_t) in WireBase
    */
   void begin(uint8_t = 0x00);

   /*
    * Sets the target bus speed, i.e. 400 kHz or 100 kHz.
    */
   void setClock(uint32_t frequencyHz);

   /*
    * Sets pins SDA and SCL to INPUT
    */
   void end();

   /*
    * If object is destroyed, set pin numbers to 0.
    */
   ~SoftWire();
};

// extern SoftWire Wire;
