#ifndef SENSORSSERIALIZER_H
#define SENSORSSERIALIZER_H

#include "ClearableBuffer.h"
#include "SensorType.h"

class SensorsSerializer{
public:
    SensorsSerializer(ClearableBuffer * jsonBuffer) : jsonBuffer(jsonBuffer)
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

    size_t printTo(Print & out){
        size_t written = root->printTo(out);
        written += out.println();

        reset();

        return written;
    }

    void reset(){
        jsonBuffer->clear();
        root = &jsonBuffer->createObject();
    }

protected:
    ClearableBuffer * jsonBuffer;
    JsonObject * root;
};

#endif // SENSORSSERIALIZER_H
