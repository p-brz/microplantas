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

//
// Hardware configuration
//
/* NRF24
    Uno   Mega
CE    9   41     (opcional)
CSN   10  40    (opcional)
SCK   13  52
MOSI  11  51
MISO  12  50

*/
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega, Mega ADK
#define CE 41
#define CSN 40
#else
#define CE 9
#define CSN 10
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

#define BUF_SIZE 64

class Buffer{
public:
  Buffer() : idx(0)
  {}
  
  bool put(const void * data, int length){
    if(length + idx > BUF_SIZE){
      return false;
    }
    
    for(int i = 0; i < index; ++i){
      buffer[idx] = data[i];
    }
    return true;
  }
  
  bool get(void * data, int length){
    if(length + idx > BUF_SIZE){
      return false;
    }
    
    for(int i = 0; i < index; ++i){
      data[i] = buffer[idx];
    }
    return true;
  }
  
  int index() const{ 
    return idx;
  }
  
  void reset(){
    idx = 0;
  }

  const byte * data(){
      return buffer;
  }
  int size() const{
    return idx;
  }
  
private:
  byte buffer[BUF_SIZE];
  int idx; //aponta onde sera real
}

class Sender{
public:
  Sender(Buffer * buf) : buffer(buf)
  {}
  
  void write(int lenght, const void * data){
    
  }
  int send(){
    radio.write(&length, sizeof(length));
    radio.writeBlocking(data,length,1000); //Wait up to 1 second to write 1 payload to the buffers
    if(radio.write(buffer->data(), );                   //Wait up to 1 second for the payload to send. Return 1 if ok, 0 if failed.
                                       //Blocks only until user timeout or success. Data flushed on fail.

  }

private:
  Buffer * buffer;
}

class Receiver{
public:  
  Receiver(Buffer * buf) : buffer(buf)
  {}
  
  bool available();
  int read();
  bool get(int bufferLength, void * outData);

private:
  Buffer * buffer;
}

void setup(void){
  Serial.begin(9600);
  printf_begin();
  printf("\n\rRF24/examples/led_remote/\n\r");

  pinMode(SENSOR_PIN, INPUT);

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
    
    radio.writeFast(&length, sizeof(length));
    radio.writeBlocking(data,length,1000); //Wait up to 1 second to write 1 payload to the buffers
    return radio.txStandBy(1000);                   //Wait up to 1 second for the payload to send. Return 1 if ok, 0 if failed.
                                       //Blocks only until user timeout or success. Data flushed on fail.
}

void emitterLoop(){
    int value = analogRead(SENSOR_PIN);
    Serial.print("read: "); Serial.println(value);
    
    bool sent = sendBytes(&value, sizeof(value));
    if (sent){
        printf("ok\n\r");
    }else{
        printf("failed\n\r");
    }
    
    // Try again in a short while
    delay(SENSOR_PERIOD);
}

void receiverLoop(){
  if ( radio.available()){
    /*int length;
    radio.read(&length, sizeof(int));
    Serial.print("Data size: "); Serial.println(length);
    byte data[length];
    radio.read(data, length);*/
    
    int length = 64;
    byte data[length];
    radio.read(data, length);
    
    Serial.print("Data: ");
    for(int i=0; i < length; ++i){
      Serial.print(data[i], HEX);
    }
    Serial.println();
  }
}
