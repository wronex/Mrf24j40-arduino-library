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

const int pin_reset = 6;
const int pin_cs = 10; // default CS pin on ATmega8/168/328
const int pin_interrupt = 2; // default interrupt pin on ATmega8/168/328

Mrf24j mrf(pin_reset, pin_cs, pin_interrupt);

long last_time = 0;
long tx_interval = 1000;

void setup() {
  Serial.begin(9600);

  mrf.init();

  mrf.set_pan(0xcafe);
  mrf.address16_write(0x6003); // this is _our_ address

  // uncomment if you want to receive any packet on this channel
  //mrf.set_promiscuous(true);

  // uncomment if you want to enable PA/LNA external control
  //mrf.set_palna(true);

  attachInterrupt(0, interrupt_routine, CHANGE); // interrupt 0 equivalent to pin 2(INT0) on ATmega8/168/328
  interrupts();
}

void interrupt_routine() {
    mrf.interrupt_handler(); // mrf24 object interrupt routine
}

void loop() {
    mrf.check_flags(&handle_rx, &handle_tx);
    unsigned long current_time = millis();
    if (current_time - last_time > tx_interval) {
        last_time = current_time;

        Serial.println("Transmitting data...");

        char str_to_send[] = "abcd";
        word receiver_address = 0x6001;
        mrf.send16(receiver_address, str_to_send, strlen(str_to_send));
    }
}

void handle_rx() {
    Serial.print("received a packet ");
    Serial.print(mrf.get_rxinfo()->frame_length, DEC);
    Serial.println(" bytes long");

    Serial.println("\r\nASCII data (relevant data):");
    for (int i = 0; i < mrf.rx_datalength(); i++) {
        Serial.write(mrf.get_rxinfo()->rx_data[i]);
    }

    Serial.print("\r\nLQI/RSSI=");
    Serial.print(mrf.get_rxinfo()->lqi, DEC);
    Serial.print("/");
    Serial.println(mrf.get_rxinfo()->rssi, DEC);
}

void handle_tx() {
    if (mrf.get_txinfo()->tx_ok) {
        Serial.println("TX went ok, got ack");
    } else {
        Serial.print("TX failed after ");
        Serial.print(mrf.get_txinfo()->retries);
        Serial.println(" retries\n");
    }
}
