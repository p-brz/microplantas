#ifndef BASICSENSORREADER_H
#define BASICSENSORREADER_H

#include "SensorReader.h"

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

#endif // BASICSENSORREADER_H
