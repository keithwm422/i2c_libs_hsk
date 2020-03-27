// try out multiple I2C twowire libs
#include "driverlib/uart.h"
#include "inc/hw_nvic.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inc/hw_i2c.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_pwm.h>
#include <driverlib/i2c.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include <driverlib/rom.h>
#include <driverlib/rom_map.h>
#include <driverlib/udma.h>
#include <driverlib/pwm.h>
#include <driverlib/ssi.h>
#include <driverlib/systick.h>
#include <driverlib/adc.h>
#include <string.h>
#include "Wire_nonblocking.h"
#define  WIRE_INTERFACES_COUNT 4
unsigned long  i2c_1 = 1;
unsigned long i2c_3 = 3;
uint8_t quantity = 2;
size_t write_num_bytes;
// declare 2 two wire objs
TwoWire *wire_1 = new TwoWire(i2c_1); // i2C object for the i2c port on the launchpad
TwoWire *wire_2 = new TwoWire(i2c_3); // i2C object for the i2c port on the launchpad
#define SLAVE_ADDR_1 32
#define SLAVE_ADDR_2 33
char one_byte;
uint8_t to_write;
int i = 0;


void setup() {
  Serial.begin(115200);
  Serial.print("Hello, starting...");
  Serial.print("\n");
  wire_2->begin();
  wire_1->begin();
}

void loop() {
  if (Serial.available() > 0) one_byte = Serial.read();
  //    Serial.print("\n");
  //    Serial.print("try writing i to test_prio_array? \n");
  //    memcpy(test_prio_array,&i, sizeof(i));
  if (one_byte == 49) Serial.print("1\n");
  else if (one_byte == 50) { //2
    Serial.print(checkRX());
    Serial.print(" is RX status of slave 1\n");
    one_byte = 69;
  }
  else if (one_byte == 51) { //3
    Serial.print(checkTX());
    Serial.print(" is TX status of slave 1\n");
    one_byte = 69;
  }
  else if (one_byte == 52) { //4
    Serial.print(wire_1->requestFrom_nonblocking(SLAVE_ADDR_1, quantity, 1));
    Serial.print(" is return from request of slave 1\n");
    one_byte = 69;
  }
  else if (one_byte == 53) { // 5
    Serial.print(wire_1->available());
    Serial.print(" is return available from slave 1\n");
    one_byte = 69;
  }
  else if (one_byte == 54) { //6
    Serial.print(wire_1->read());
    Serial.print(" was read from slave 1\n");
    one_byte = 69;
  }
  else if (one_byte == 55) { //7
    if(to_write>=255) to_write=0;
    else to_write+=11;
    wire_1->beginTransmission(SLAVE_ADDR_1); // needs to be nonblocking?
    wire_1->write(to_write);
    Serial.print(wire_1->endTransmission_nonblocking(1));
    Serial.print(" is status after endtransmission on slave 1\n");
    one_byte = 69;
  }

    else if (one_byte == 56) { //8
    Serial.print(wire_2->getRxStatus(quantity));
    Serial.print(" is RX status of slave 2\n");
    one_byte = 69;
  }
  else if (one_byte == 57) { //9
    Serial.print(wire_2->getTxStatus());
    Serial.print(" is TX status of slave 2\n");
    one_byte = 69;
  }
  else if (one_byte == 65) { //A
    Serial.print(wire_2->requestFrom_nonblocking(SLAVE_ADDR_2, quantity, 1));
    Serial.print(" is return from request of slave 2\n");
    one_byte = 69;
  }
  else if (one_byte == 66) { //B
    Serial.print(wire_2->available());
    Serial.print(" is return available from slave 2\n");
    one_byte = 69;
  }
  else if (one_byte == 67) { //C
    Serial.print(wire_2->read());
    Serial.print(" was read from slave 2\n");
    one_byte = 69;
  }
  else if (one_byte == 68) { //D
    if(to_write>=255) to_write=0;
    else to_write+=11;
    wire_2->beginTransmission(SLAVE_ADDR_2); // needs to be nonblocking?
    wire_2->write(to_write);
    Serial.print(wire_2->endTransmission_nonblocking(1));
    Serial.print(" is status after endtransmission on slave 2\n");
    one_byte = 69;
  }
  else {
    Serial.print("x");
  }
  delay(1000);
}

uint8_t checkRX(){  
  return wire_1->getRxStatus(quantity);
}

uint8_t checkTX(){  
  return wire_1->getTxStatus();
}
