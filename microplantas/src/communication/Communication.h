#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Arduino.h"
#include "ArduinoJson.h"

#include "Sender.h"
#include "Receiver.h"
#include "RFAdapter.h"

#include "printf.h"

template<typename Receiver>
char * receiveString(Receiver & receiver){
    auto len = receiver.read();
    receiver.data()[len] = '\0';

    return (char *)receiver.data();
}
template<typename Receiver>
JsonObject & receiveJsonObject(Receiver & receiver, JsonBuffer * buffer){
    return buffer->parseObject(receiveString(receiver));
}

template<typename Sender>
size_t sendObject(JsonObject & obj, Sender & sender){
    sender.reset();

    size_t written = obj.printTo(sender);
    written += sender.println();

    return sender.send();
}

class CommunicationRF{
public:
    CommunicationRF(int CE, int CSN)
        : radio(CE, CSN)
        , rfAdapter(&radio)
        , buffer()
        , sender(&rfAdapter, &buffer)
        , receiver(&rfAdapter, &buffer)
    {}

    int available() const{
        return rfAdapter.available();
    }

    void begin(const uint64_t readingPipe, const uint64_t writingPipe){
        printf_begin();

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

        radio.printDetails();
    }

    JsonObject & readJsonObject(JsonBuffer * buffer){
        return receiveJsonObject(receiver, buffer);
    }

    size_t sendJson(JsonObject &obj){
        return sendObject(obj, this->sender);
    }

protected:
    RF24 radio;
    RFAdapter rfAdapter;
    Buffer buffer;

public:
    Sender<RFAdapter> sender;
    Receiver<RFAdapter> receiver;
};

class SerialReceiver{
public:
  int available() const{
    return Serial.available();
  }

  int readBytes(byte * data, int capacity){
  //            char msg[150];
//            int r = Serial.readBytesUntil('\n', msg, 150);
//            msg[r] = 0;
    return Serial.readBytesUntil('\n', (char *)data, capacity);
  }

};

class CommunicationSerial{
public:
    CommunicationSerial()
        : buffer()
        , serialReceiverAdapter()
        , sender(&Serial, &buffer)
        , receiver(&serialReceiverAdapter, &buffer)
    {}

    void begin(const uint64_t readingPipe, const uint64_t writingPipe)
    {
        //keep only to mantain compatibility with CommunicationRF
    }

    int available() const{
        return Serial.available();
    }

    JsonObject & readJsonObject(JsonBuffer * buffer){
        return receiveJsonObject(receiver, buffer);
    }

    size_t sendJson(JsonObject &obj){
        return sendObject(obj, this->sender);
    }

protected:
    Buffer buffer;

public:
    SerialReceiver serialReceiverAdapter;
    Sender<HardwareSerial> sender;
    Receiver<SerialReceiver> receiver;
};

#endif // COMMUNICATION_H
