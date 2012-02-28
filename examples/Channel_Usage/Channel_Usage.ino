/**
 * Example code for using a microchip mrf24j40 module to send and receive
 * packets using plain 802.15.4
 * Requirements: 3 pins for spi, 3 pins for reset, chip select and interrupt
 * notifications
 * This example file is considered to be in the public domain
 * Originally written by Karl Palsson, karlp@tweak.net.au, March 2011
 * Modified by Chris Dunn, chris@dunns.net, Feb 2012
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

int rssi_high[16];
int rssi_average_total[16];
int rssi_total_samples = 0;
int current_rssi_reading = 0;

long sample_interval = 4000;

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
  unsigned long sample_end = millis() + sample_interval;
  while (millis() <= sample_end) {
    for (int i = 0; i <= 15; i++) { //Channel 11 - 26
      channel_number = i + 11;
      mrf.set_channel(channel_number);
      mrf.write_short(MRF_RFCTL, 0x04); //  – Reset RF state machine.
      mrf.write_short(MRF_RFCTL, 0x00); // part 2
      delay(1); // delay at least 192usec
      mrf.write_short(MRF_BBREG6, 0x80);
      while(rssi_calc != 1){
        rssi_calc = mrf.read_short(MRF_BBREG6), BIN;
      }
      current_rssi_reading = mrf.read_long(0x210), DEC;
      rssi_average_total[i] = rssi_average_total[i] + (current_rssi_reading / 3);
    
      if (rssi_high[i] < current_rssi_reading){
        rssi_high[i] = current_rssi_reading / 3;
      }
  
      rssi_calc = 0;
      
    }
    rssi_total_samples++;
  }
  Serial.println("    Channel Sampling Output");
  Serial.print("    Total samples: ");
  Serial.println(rssi_total_samples);
  //Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
  for (int i = 0; i <= 15; i++) { //Channel 11 - 26
    Serial.print("Channel ");
    Serial.print(i + 11);
    Serial.print(" | ");
      for (int z = 0; z <= rssi_high[i]; z++) {
        if (z < (rssi_average_total[i] / rssi_total_samples)){
          Serial.print("#");
        }
        else{
          Serial.print("*");
        }
      }
      Serial.print("   ");
      Serial.print(rssi_average_total[i] / rssi_total_samples);
      Serial.print(" | ");
      Serial.println(rssi_high[i]);
    //Serial.print(rssi_high[i]);
    //Serial.print(" | ");
    //Serial.print(rssi_average_total[i]);
    //Serial.print(" | ");
    //Serial.println(rssi_average_total[i] / rssi_total_samples);
  }
  Serial.println(" ");
  delay(1000);
  for (int i = 0; i <= 15; i++) {
    rssi_high[i] = 0;
    rssi_average_total[i] = 0;
  }
  rssi_total_samples = 0;
}
