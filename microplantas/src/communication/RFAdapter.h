#ifndef RFADAPTER_H
#define RFADAPTER_H

#include <RF24.h>
#include <printf.h>

#include "Buffer.h"
#include "helpers/Timer.h"

class RFAdapter{
public:
    static constexpr const int PKG_SIZE = 32;

public:
    RFAdapter(RF24 * radio)
        : radio(radio)
    {}

    void begin(const uint64_t readingPipe, const uint64_t writingPipe){
        this->readingPipe = readingPipe;
        this->writingPipe = writingPipe;

        printf_begin();

        radio->begin();

        // enable dynamic payloads
        radio->enableDynamicPayloads();

        if (writingPipe){
            radio->openWritingPipe(writingPipe);
        }
        if(readingPipe){
            radio->openReadingPipe(1,readingPipe);
            radio->startListening();
        }

        radio->printDetails();
    }

    int available() const{
        if(!radio->available()){
            return 0;
        }

        return radio->getDynamicPayloadSize();
    }

    bool write(const uint8_t *data, size_t size){
        if(!writingPipe){
            return false;
        }

        bool success = true;

        if(readingPipe){
            radio->stopListening();
        }

        int dataToSend = 0;
        for (size_t sendCount = 0; sendCount < size; sendCount += dataToSend) {
            dataToSend = size - sendCount;
            bool more = (dataToSend >= PKG_SIZE);
            if(more){
                dataToSend = PKG_SIZE - 1;
            }

            buffer.reset();
            buffer.put(data + sendCount, dataToSend);
            if(more){
                buffer.put(1);
            }

            radio->write(buffer.data(), buffer.size());
            if(!radio->txStandBy()){
                success = false;
                break;
            }
        }

        if(readingPipe){
            radio->startListening();
        }

        return success;
    }

    int readBytes(byte * data, int capacity){
        int count = 0;

        int length = available();

        while(length > 0){
            bool more = (length == PKG_SIZE);
            if(more){
                --length;
            }

            buffer.reset();
            radio->read(buffer.data(), buffer.capacity());
            buffer.size(length);

            length = min(length, capacity);
            buffer.get(data + count, length);

            count += length;
            capacity -= length;

            if(!more || capacity <= 0){
                break;
            }

            length = waitUntilAvailable();
        }

        return count;
    }

protected:
    int waitUntilAvailable(unsigned timeout=200){
        Timer timer{timeout};

        int length = 0;

        timer.start();
        while(length == 0 && !timer.finished()){
            length = available();
        }

        return length;
    }

protected:
    RF24 * radio;
    uint64_t readingPipe;
    uint64_t writingPipe;
    GenericBuffer<32> buffer;
};

#endif // RFADAPTER_H
