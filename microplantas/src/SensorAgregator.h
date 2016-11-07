#ifndef SENSOR_AGREGATOR_H_
#define SENSOR_AGREGATOR_H_

#include "protocol.h"
#include "rf_communication.h"

class SensorReader{
public:
  using Id = uint32_t;

  virtual int count() = 0;
  virtual bool read(int sensor, int * outValue, SensorType * outType)=0;

  Id id() const{
    return _id;
  }
  SensorReader & id(Id id){
    this->_id = id;
    return *this;
  }

private:
  Id _id;
};

typedef int (*ReadFunction)(byte);

template<size_t CAPACITY>
class BasicSensorReader : public SensorReader{
public:
  int count() override{
    return size;
  }
  bool read(int sensor, int * outValue, SensorType * outType){
    if(sensor < 0 || sensor >= size || outValue == NULL || outType == NULL){
      return false;
    }
//    *outValue = analogRead(pins[sensor]);
    *outValue = funcPointers[sensor](pins[sensor]);
    *outType = pinType[sensor];
    return true;
  }

  void addSensor(int pin, SensorType type, ReadFunction functor=NULL){
    if(size + 1 > CAPACITY){
      return;
    }

    pins[size] = pin;
    pinType[size] = type;
    funcPointers[size] = (functor == NULL) ? analogRead : functor;

    ++size;
  }
  void removeSensor(int pin){
    for(int i=0; i < size; ++i){
      if(pins[i] == pin){
        pins[i] = pins[size];
        pinType[i] = pinType[size];
        --size;
        --i;
      }
    }
  }

protected:
  int        pins[CAPACITY];
  SensorType pinType[CAPACITY];
  ReadFunction funcPointers[CAPACITY];
  int size =0;
};

template<size_t CAPACITY=3, size_t BUFFER_SIZE=200>
class SensorAgregator{
public:
  using Id = uint32_t;

  SensorAgregator(Communication * comm)
    : comm(comm)
  {}

  bool addNode(Id id, SensorReader * sensor){
    if(numSensors == CAPACITY){
      return false;
    }

    sensor->id(id);
    sensors[numSensors] = sensor;
    ++numSensors;

    return true;
  }

  int count() const{
    return numSensors;
  }

  bool sendData(unsigned index){
    if(index >= numSensors){
      return false;
    }

    SensorReader * reader = sensors[index];
    serializer.beginData(reader->id());

    int value;
    SensorType type;

    for(int i=0; i < reader->count(); ++i){
      if(reader->read(i, &value, &type)){
        serializer.add(type, value);
      }
    }

    if(serializer.printTo(comm->sender)){
      return comm->sender.send() > 0;
    }

    return false;
  }

protected:
  Communication * comm;
  StaticJBuffer<BUFFER_SIZE> serial_buffer;
  SensorsSerializer serializer{&serial_buffer};
  SensorReader * sensors[CAPACITY];
  int numSensors=0;
};

#endif
