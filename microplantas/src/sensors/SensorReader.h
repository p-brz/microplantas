#ifndef SENSORREADER_H
#define SENSORREADER_H

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

#endif // SENSORREADER_H
