#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Sender.h"
#include "Receiver.h"
#include "RFAdapter.h"

class CommunicationRF{
public:
  CommunicationRF(int CE, int CSN)
    : radio(CE, CSN)
    , rfAdapter(&radio)
    , buffer()
    , sender(&rfAdapter, &buffer)
    , receiver(&rfAdapter, &buffer)
  {}

  void begin(const uint64_t readingPipe, const uint64_t writingPipe){
    radio.begin();

    // enable dynamic payloads
    radio.enableDynamicPayloads();

    if (writingPipe){
      radio.openWritingPipe(writingPipe);
    }
    if(readingPipe){
      radio.openReadingPipe(1,readingPipe);
      radio.startListening();
    }
  }

protected:
  RF24 radio;
  RFAdapter rfAdapter;
  Buffer buffer;

public:
  Sender<RFAdapter> sender;
  Receiver<RFAdapter> receiver;
};

class CommunicationSerial{
public:
  CommunicationSerial()
    : buffer()
    , sender(&Serial, &buffer)
    , receiver(&Serial, &buffer)
  {}

  void begin(const uint64_t readingPipe, const uint64_t writingPipe){

  }

protected:
  Buffer buffer;

public:
  Sender<HardwareSerial> sender;
  Receiver<HardwareSerial> receiver;
};

#endif // COMMUNICATION_H
