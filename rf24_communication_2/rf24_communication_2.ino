/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#include "Buffer.h"

//
// Hardware configuration
//
/* NRF24
    Uno   Mega
CE    9   41     (opcional)
CSN   8  40    (opcional)
SCK   13  52
MOSI  11  51
MISO  12  50

*/
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega, Mega ADK
#define CE 41
#define CSN 40
#else
#define CE 9
#define CSN 8
#endif

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 (CE & CS)

RF24 radio(CE,CSN);

// Single radio pipe address for the 2 nodes to communicate.
//const uint64_t pipe = 0xE8E8F0F0E2LL;
const uint64_t pipe = 0xE13CBAF433LL;

// The various roles supported by this sketch
typedef enum { role_emitter = 1, role_receiver } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote", "Base"};

// The role of the current running sketch
role_e role;


#define SENSOR_PIN A0
//leituras a cada 1 segundo
#define SENSOR_PERIOD 1000

void setupRole();
void beginRadio();

void emitterLoop();
void receiverLoop();

class RFAdapter{
public:
  RFAdapter(RF24 * radio) : radio(radio)
  {}

  int available() const{
    if(!radio->available()){
      return 0;
    }
    
    return radio->getDynamicPayloadSize();
  }

  bool write(const uint8_t *data, size_t size){
    radio->write(data, size);
    return radio->txStandBy();
  }

  void read(byte * data, int capacity){
    radio->read(data, capacity);
  }

protected:
  RF24 * radio;
};

Buffer buffer;
RFAdapter rfAdapter{&radio};
Sender<RFAdapter> sender{&rfAdapter, & buffer};
Receiver<RFAdapter> receiver{&rfAdapter, &buffer};


void setup(void){
  Serial.begin(9600);
  printf_begin();
  printf("\n\rRF24/examples/led_remote/\n\r");

  pinMode(SENSOR_PIN, INPUT);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  setupRole();
  beginRadio();
}

//
// Loop
//
void loop(void){
  if ( role == role_emitter ){
    emitterLoop();
  }
  else if ( role == role_receiver ){
    receiverLoop();
  }
}


void setupRole(){
  Serial.println("ROLE: emissor(1), receptor(0)?");
  int r;
  while(!Serial.available()){
    delay(50);
  }
  r= Serial.read();

  if(r == '1'){
    role = role_emitter;
  }
  else{
    role = role_receiver;
  }
  printf("ROLE: %s\n\r",role_friendly_name[role]);
}

void beginRadio(){
  
  radio.begin();
  // enable dynamic payloads
  radio.enableDynamicPayloads();
//  radio.enableDynamicAck();
//  radio.setAutoAck(true);

  if ( role == role_emitter )
  {
    radio.openWritingPipe(pipe);
  }
  else
  {
    radio.openReadingPipe(1,pipe);
  }

  if ( role == role_receiver )
    radio.startListening();

  radio.printDetails();
}

bool sendBytes(void * data, int length){
    printf("Now sending... %d bytes\n", length);
    return radio.write(data, length);
}

void emitterLoop(){
    int value = analogRead(SENSOR_PIN);
    Serial.print("analog read: "); Serial.println(value);
    
//    bool sent = sendBytes(&value, sizeof(value));

    sender.print("value: "); sender.print(value);
    int sBytes = sender.send();
    Serial.print("sent ");Serial.print(sBytes); Serial.println(" bytes");
//    if ( > 0){
//        printf("ok\n\r");
//    }else{
//        printf("failed\n\r");
//    }
    
    // Try again in a short while
    delay(SENSOR_PERIOD);
}

void receiverLoop(){
  if ( receiver.available()){
    
    uint8_t len = receiver.read();
      
    // If a corrupt dynamic payload is received, it will be flushed
    if(!len){
      Serial.println("Got corrupt package (0 bytes)");
      return; 
    }
    else{
      Serial.print("Received "); Serial.print(len); Serial.println(" bytes");
    }
      
    byte * data = receiver.data();
    
    Serial.print("Data: \"");
    for(int i=0; i < len; ++i){
//      Serial.print(data[i - 1], HEX);
      Serial.print((char)data[i]);
    }
    Serial.println("\"");
  }
}

