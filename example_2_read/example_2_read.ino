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
#include <Wire_nonblocking.h>
#define  WIRE_INTERFACES_COUNT 4  // to use all 4 I2Cs if necessary
#define SENSOR_UPDATE_PERIOD 1000  // period between reads/writes
#define SENSOR_UPDATE_PERIOD_TX 1500  // period between reads/writes
#define QUANTITY_BYTES 2
uint8_t quantity = 2;

unsigned long  i2c_1 = 1;
unsigned long i2c_3 = 3;
unsigned long sensorUpdateTime_1_tx=0;  // for transmit recording time
unsigned long sensorUpdateTime_2_tx=0;  //for transmit recording time
unsigned long sensorUpdateTime_1_rx=0;  // for reading time
unsigned long sensorUpdateTime_2_rx=0;  // reading time
size_t write_num_bytes;
// declare 2 two wire objs
TwoWire *wire_1 = new TwoWire(i2c_1); // i2C object for the i2c port on the launchpad
TwoWire *wire_2 = new TwoWire(i2c_3); // i2C object for the i2c port on the launchpad
#define SLAVE_ADDR_1 32
#define SLAVE_ADDR_2 33
char one_byte;
uint8_t to_write_1=0;
uint8_t to_write_2=0;
// states needed for writing
typedef enum {
  SENSOR_STATE_I2C_IDLE = 0, // checks for busy on either tx or rx and either transmits or requests if not busy on either.
  SENSOR_STATE_I2C_COMPLETE = 1, // once a tx or rx transaction complete, falls to wait
  SENSOR_STATE_I2C_WAIT = 2 // waits until sensor update time is over
} SensorState;
SensorState sState_1_tx = SENSOR_STATE_I2C_IDLE;
SensorState sState_2_tx = SENSOR_STATE_I2C_IDLE;
SensorState sState_1_rx = SENSOR_STATE_I2C_IDLE;
SensorState sState_2_rx = SENSOR_STATE_I2C_IDLE;
uint8_t read_values_1[QUANTITY_BYTES];
uint8_t read_values_2[QUANTITY_BYTES];

int i =0;
// ONLY READ/TRANSMIT when both are not busy. Else bus will be busy all the time. 


void setup() {
  Serial.begin(115200);
  Serial.print("Hello, starting...");
  Serial.print("\n");
  wire_2->begin();
  wire_1->begin();
}

void loop() {
  // TX of SLAVE 1
  switch(sState_1_tx){
    case SENSOR_STATE_I2C_IDLE:{
      // if both status' are not busy then transmit
      if(wire_1->getTxStatus()!=I2C_BUSY && wire_1->getRxStatus(quantity)!=I2C_BUSY){
        wire_1->beginTransmission(SLAVE_ADDR_1); // needs to be nonblocking?
        wire_1->write(to_write_1);
        wire_1->endTransmission_nonblocking(1);
        sState_1_tx = (SensorState) ((unsigned char) sState_1_tx + 1);
        to_write_1+=11;
        break;  // break out to give uC time to do other stuff
      }
      else break;
    }
    case SENSOR_STATE_I2C_COMPLETE:{
      sensorUpdateTime_1_tx = millis() + SENSOR_UPDATE_PERIOD_TX;
      sState_1_tx = SENSOR_STATE_I2C_WAIT;
    }
    case SENSOR_STATE_I2C_WAIT:{
      if ((long) (millis() - sensorUpdateTime_1_tx) > 0) {
        sState_1_tx = SENSOR_STATE_I2C_IDLE;
        break;
      }
    }
  }
  // RX of SLAVE 1
  switch (sState_1_rx){
    case SENSOR_STATE_I2C_IDLE:{
      // if both status' are not busy then ask for a read
      if(wire_1->getTxStatus()!=I2C_BUSY && wire_1->getRxStatus(quantity)!=I2C_BUSY){
        wire_1->requestFrom_nonblocking(SLAVE_ADDR_1, quantity, 1);
        sState_1_rx = (SensorState) ((unsigned char) sState_1_rx + 1);
      }
      else break; 
    }
    case SENSOR_STATE_I2C_COMPLETE:{
      // in reads, need to check the rx for busy again until its 0 or else it bad things happen. 
      if(wire_1->getTxStatus()!=I2C_BUSY && wire_1->getRxStatus(quantity)!=I2C_BUSY){
        i=0;
        while(wire_1->available()){
          read_values_1[i]=wire_1->read();
          Serial.print("slave 1 read value: ");
          Serial.print(read_values_1[i]);
          Serial.print("\n");
          i++;
        }
        sensorUpdateTime_1_rx = millis() + SENSOR_UPDATE_PERIOD;
        sState_1_rx = SENSOR_STATE_I2C_WAIT;
      }
      else break;
    }
    case SENSOR_STATE_I2C_WAIT:{
      if ((long) (millis() - sensorUpdateTime_1_rx) > 0) {
        sState_1_rx = SENSOR_STATE_I2C_IDLE;
        break;
      }
    }
  }
  // TX of SLAVE 2
  switch(sState_2_tx){
    case SENSOR_STATE_I2C_IDLE:{
      // if both status' are not busy then transmit
      if(wire_2->getTxStatus()!=I2C_BUSY && wire_2->getRxStatus(quantity)!=I2C_BUSY){
        wire_2->beginTransmission(SLAVE_ADDR_2); // needs to be nonblocking?
        wire_2->write(to_write_2);
        wire_2->endTransmission_nonblocking(1);
        sState_2_tx = (SensorState) ((unsigned char) sState_2_tx + 1);
        to_write_2+=11;
        break;  // break out to give uC time to do other stuff
      }
      else break;
    }
    case SENSOR_STATE_I2C_COMPLETE:{
      sensorUpdateTime_2_tx = millis() + SENSOR_UPDATE_PERIOD_TX;
      sState_2_tx = SENSOR_STATE_I2C_WAIT;
    }
    case SENSOR_STATE_I2C_WAIT:{
      if ((long) (millis() - sensorUpdateTime_2_tx) > 0) {
        sState_2_tx = SENSOR_STATE_I2C_IDLE;
        break;
      }
    }
  }
  // RX of SLAVE 2
  switch (sState_2_rx){
    case SENSOR_STATE_I2C_IDLE:{
      // if both status' are not busy then ask for a read
      if(wire_2->getTxStatus()!=I2C_BUSY && wire_2->getRxStatus(quantity)!=I2C_BUSY){
        wire_2->requestFrom_nonblocking(SLAVE_ADDR_2, quantity, 1);
        sState_2_rx = (SensorState) ((unsigned char) sState_2_rx + 1);
      }
      else break; 
    }
    case SENSOR_STATE_I2C_COMPLETE:{
      // in reads, need to check the rx for busy again until its 0 or else it bad things happen. 
      if(wire_2->getTxStatus()!=I2C_BUSY && wire_2->getRxStatus(quantity)!=I2C_BUSY){
        i=0;
        while(wire_2->available()){
          read_values_2[i]=wire_2->read();
          Serial.print("slave 2 read value: ");
          Serial.print(read_values_2[i]);
          Serial.print("\n");
          i++;
        }
        sensorUpdateTime_2_rx = millis() + SENSOR_UPDATE_PERIOD;
        sState_2_rx = SENSOR_STATE_I2C_WAIT;
      }
      else break;
    }
    case SENSOR_STATE_I2C_WAIT:{
      if ((long) (millis() - sensorUpdateTime_2_rx) > 0) {
        sState_2_rx = SENSOR_STATE_I2C_IDLE;
        break;
      }
    }
  }
}
