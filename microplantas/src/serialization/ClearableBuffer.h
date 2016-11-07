#ifndef CLEARABLEBUFFER_H
#define CLEARABLEBUFFER_H

#include <ArduinoJson.h>

class ClearableBuffer : public JsonBuffer{
public:
  virtual void clear()=0;
};

template <size_t CAPACITY>
class StaticJBuffer : public ClearableBuffer {
 public:
  explicit StaticJBuffer() : _size(0) {}

  size_t capacity() const {
    return CAPACITY;
  }
  size_t size() const {
    return _size;
  }

  void clear() override{
    memset(_buffer, 0, CAPACITY);
    this->_size = 0;
  }

  virtual void* alloc(size_t bytes) {
    if (_size + bytes > CAPACITY) return NULL;
    void* p = &_buffer[_size];
    _size += round_size_up(bytes);
    return p;
  }

 protected:
  uint8_t _buffer[CAPACITY];
  size_t _size;
};

#endif // CLEARABLEBUFFER_H
