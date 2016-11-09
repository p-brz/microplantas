#ifndef RFADAPTER_H
#define RFADAPTER_H

#include <RF24.h>

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

    int readBytes(byte * data, int capacity){
        int length = available();
        radio->read(data, capacity);
        return length;
    }

protected:
    RF24 * radio;
};

#endif // RFADAPTER_H
