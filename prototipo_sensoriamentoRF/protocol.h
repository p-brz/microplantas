#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <ArduinoJson.h>

enum class SensorType{
  light = 1,
  temperature
};

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

class SensorsSerializer{
public:
  SensorsSerializer(ClearableBuffer * buffer) 
    : jsonBuffer(buffer)
  {}
  
  void beginData(uint32_t nodeId){
    reset();
    root->set("id", nodeId);
  }

  const char * getTypeName(SensorType type){
    switch(type){
      case SensorType::light:
        return "light";
      case SensorType::temperature:
        return "temp";
      default:
        return "unknown";
    }
  }
  void add(SensorType type, uint16_t value){
    root->set(getTypeName(type), value);
  }

  bool printTo(Print & out){
    root->printTo(out);
    out.println();

    reset();
    
    return true;
  }

  void reset(){
    jsonBuffer->clear();
    root = &jsonBuffer->createObject();
  }
  
protected:
  ClearableBuffer * jsonBuffer;
  JsonObject * root;
};



#endif
