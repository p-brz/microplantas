#ifndef RECEIVER_H
#define RECEIVER_H

#include "Buffer.h"

template<typename InputT>
class Receiver{
public:
  Receiver(InputT * in, Buffer * buf)
    : input(in), buffer(buf)
  {}

  bool available(){
    return input->available() > 0;
  }

  int read(){
    int length = input->available();

    if(length > 0){
      buffer->reset();
      length = input->readBytes(buffer->data(), buffer->capacity());
      buffer->size(length);
    }

    return length;
  }
  bool get(void * outData, int bufferLength){
    return buffer->get(outData, bufferLength);
  }

  const byte * data() const{
      return buffer->data();
  }
  byte * data(){
      return buffer->data();
  }
private:
  InputT * input;
  Buffer * buffer;
};

#endif // RECEIVER_H
