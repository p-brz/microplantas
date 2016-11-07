#ifndef SENSORAGREGATOR_H
#define SENSORAGREGATOR_H

#include "protocol.h"
#include "communication.h"
#include "SensorReader.h"

template<typename Comm, size_t CAPACITY=3, size_t BUFFER_SIZE=200>
class SensorAgregator{
public:
  using Id = uint32_t;

  SensorAgregator(Comm * comm)
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
  Comm * comm;
  StaticJBuffer<BUFFER_SIZE> serial_buffer;
  SensorsSerializer serializer{&serial_buffer};
  SensorReader * sensors[CAPACITY];
  int numSensors=0;
};

#endif // SENSORAGREGATOR_H
