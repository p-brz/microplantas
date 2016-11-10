#ifndef SENDER_H
#define SENDER_H

#include "Arduino.h"
#include "Buffer.h"

template<typename WriterT>
class Sender : public Print{
public:
    Sender(WriterT * out, Buffer * buf)
        : out(out), buffer(buf)
    {
        reset();
    }

    size_t write(uint8_t data) override{
        return this->write(&data, 1);
    }
    size_t write(const uint8_t *data, size_t size) override{
        return buffer->put(data, size) ? size : 0;
    }
    //  bool write(const void * data, int length){
    //    return buffer->put(data, length);
    //  }
    int send(){
        int wroteBytes = 0;
        if(out->write(buffer->data(), buffer->size())){
            wroteBytes = buffer->size();
        }
        reset();
        return wroteBytes;
    }

    void reset(){
        buffer->reset();
    }
public:
    WriterT * out;
    Buffer * buffer;
};

#endif // SENDER_H
