#pragma once

#include "ArduinoJson.h"

using Variant = JsonObject;

class Service{
public:
    virtual void setup(){}
    virtual bool execute(const Variant & args){ return false;}
    virtual void update(){}
    virtual void query(Variant & out){}

    bool running() const{
        return _running;
    }


    const char * name = "";

protected:
    void setRunning(bool running){
        this->_running = running;
    }

    bool _running;
};
