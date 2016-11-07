#ifndef __BUFFER_H__
#define __BUFFER_H__
#include "Arduino.h"

#define BUF_SIZE 64

class Buffer{
public:
  Buffer() : idx(0)
  {}
  
  bool put(const void * data, int length){
    if(putAt(data, length, idx)){
      this->idx += length;
      return true;
    }
    return false;
  }
  bool putAt(const void * data, int length, int idx){
    const byte * bytes = (const byte *)data;
    
    if(length + idx > capacity() || idx < 0){
      return false;
    }
    
    for(int i = 0; i < length; ++i){
      buffer[idx] = bytes[i];
      ++idx;
    }

    if(idx > _size){
      _size = idx;
    }
    
    return true;
  }
  
  bool get(void * data, int length){
    byte * bytes = (byte *)data;
    
    if(length + idx > BUF_SIZE){
      return false;
    }
    
    for(int i = 0; i < idx; ++i){
      bytes[i] = buffer[idx];
    }
    return true;
  }
  
  
  void reset(){
    this->forward(0);
  }
  void forward(int idx){
    this->idx = idx;
  }
  int index() const{ 
    return idx;
  }

  const byte * data() const{
      return buffer;
  }
  byte * data(){
      return buffer;
  }
  int size() const{
    return _size;
  }
  void size(int length){
    this->_size = length;
  }
  int capacity() const{
    return BUF_SIZE;
  }
private:
  byte buffer[BUF_SIZE];
  int idx; //aponta onde sera lido/escrito
  int _size;
};

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
      if(out->write(buffer->data(), buffer->size())){
        reset();
        return buffer->size();
      }
      return 0;
  }

  void reset(){
    //reserve bytes for message length field
//    buffer->forward(2);
    buffer->reset();
  }
private:
  Buffer * buffer;
  WriterT * out;
};

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
      input->read(buffer->data(), length);
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

#endif
