# SoftWireSTM32

---

This library is a software implementation of the I2C bus for the latest **Arduino Core STM32** framework, which I've ported over from the **LeafLabs Maple** framework.
You'll need such a library in case you want to address I2C peripherial devices **not** using the integrated hardware I2C bus for various reasons.

The library utilizes the *digitalWriteFast()* and *digitalReadFast()* functions of the framework in order to get decent speeds on the software I2C bus.

As it's focusing on the **Arduino Core STM32** framework, it's pretty obvious that it's meant to be used on STM32 MCUs only (only).

Please be aware: *On some MCU's software I2C can't reach the speed of hardware I2C, whereas on others it may overwhelm the I2C peripherials. The max. bus speed you can achieve on a STM32F103xx MCU is about 240 kHz. This is a decent speed for driving I2C displays for example. Other IC2 hardware might not even need such a high speed. Always refer to the datasheet of your peripherals in order to figure what speed you need.
On faster MCUs such as the STM32F407xx, you may need to reduce the speed by using longer delays. Please have a look at the WEAK **I2C_Delay()** function to see how to do this.*

## Basic usage

---

To use this library in your Arduino Core STM32 project, simply place a link to this github repository into your platformio.ini's lib_deps, i.e.:

```platformio
lib_deps =  ...
            https://github.com/technik-gegg/SoftWireSTM32.git
            ...
```

In your project source code, you have to create an instance first, then set the desired bus speed before you can use it.

Here's a simple I2C bus scanner example based on this library:

```cpp
#include <Arduino.h>
#include <SoftWire.h>

#define SDA_PIN     PC14        // your I2C data pin goes here
#define SCL_PIN     PA1         // your I2C clock pin goes here

SoftWire myI2C(SDA_PIN, SCL_PIN);   // create a new instance

uint8_t scanI2CDevices();       // forward declaration for a happy compiler

void setup() {
    Serial.begin(115200);       // init serial interface
    
    myI2C.setClock(400000U);    // set full speed clock
    myI2C.begin();              // start software I2C bus
    
    uint8_t i2cDevs = scanI2CDevices(); // run the scan
    ...
}

uint8_t scanI2CDevices()
{
  uint8_t cnt = 0;
  for (uint8_t address = 1; address < 127; address++)
  {
    myI2C.beginTransmission(address);
    uint8_t stat = myI2C.endTransmission();
    if (stat == I2C_OK)
    {
      cnt++;
      Serial.print("I2C device found at address: ");
      Serial.println(address, HEX);
    }
    delay(3);
  }
  return cnt;
}
            
```

In case you need a more complex example, have a peek at my [LeoNerd's OLED Module library](https://github.com/technik-gegg/LeoNerd-OLED-Module-Library), which uses an abstration layer on the I2C bus, in order to be able to handle both, software and hardware I2C.

## Credits

---

Credit goes to all of the developers who have been involved in building / improving this library:

+ LeafLabs
+ Trystan Jones
+ gke
+ ala42
+ Roger Clark
+ Brandon Green
