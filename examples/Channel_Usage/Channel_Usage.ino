/**
 * Example code for using a microchip mrf24j40 module to send and receive
 * packets using plain 802.15.4
 * Requirements: 3 pins for spi, 3 pins for reset, chip select and interrupt
 * notifications
 * This example file is considered to be in the public domain
 * Originally written by Karl Palsson, karlp@tweak.net.au, March 2011
 */
#include <SPI.h>
#include <mrf24j.h>
#include <stdio.h>
#include <stdlib.h>

const int pin_reset = 6;
const int pin_cs = 10; // default CS pin on ATmega8/168/328
const int pin_interrupt = 2; // default interrupt pin on ATmega8/168/328

Mrf24j mrf(pin_reset, pin_cs, pin_interrupt);

int channel_number = 0;
int rssi_calc = 0;

void setup() {

  Serial.begin(9600);
  Serial.println("Unit 'B' ");
  mrf.reset();
  mrf.init();
  
  mrf.set_pan(0xcafe);
  // This is _our_ address
  mrf.address16_write(0x6002); 
  Serial.println("Start delay");
  delay(5000);
  Serial.println("MRF Config Read Back");
  Serial.print("PAN ID: ");
  Serial.println(mrf.get_pan(), HEX);
  Serial.print("Address: ");
  Serial.println(mrf.address16_read(), HEX);
  Serial.print("Channel #: ");
  Serial.println(mrf.get_channel(), HEX);

  // uncomment if you want to receive any packet on this channel
  mrf.set_promiscuous(true);
  
  // uncomment if you want to enable PA/LNA external control
  //mrf.set_palna(true);
  
  // uncomment if you want to buffer all PHY Payload
  //mrf.set_bufferPHY(true);

}

void loop() {

  for (int i = 0; i <= 15; i++) { //Channel 11 - 26
    channel_number = i + 11;
    mrf.set_channel(channel_number);
    mrf.write_short(MRF_RFCTL, 0x04); //  – Reset RF state machine.
    mrf.write_short(MRF_RFCTL, 0x00); // part 2
    delay(1); // delay at least 192usec
    Serial.print(mrf.get_channel(), HEX);
    Serial.print(" | ");
    mrf.write_short(MRF_BBREG6, 0x80);
    while(rssi_calc != 1){
        // do something repetitive 200 times
      rssi_calc = mrf.read_short(MRF_BBREG6), BIN;
    }
    //Serial.print(rssi_calc);
    //Serial.print(" | ");
    Serial.print(mrf.read_long(0x210), HEX);
    Serial.print(" | ");
    Serial.println(mrf.read_long(0x210), DEC);
    rssi_calc = 0;
  }
  Serial.println("----   ");
  delay(1000);
}

