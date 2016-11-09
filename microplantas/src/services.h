#ifndef SERVICES_H
#define SERVICES_H

#include <Arduino.h>
#include "services/Service.h"
#include "helpers/Timer.h"

class WaterService : public Service{
public:
    WaterService(const char * name, int relayPin)
        : pin(relayPin)
    {
        this->name = name;
    }

    void setup() override{
        pinMode(pin, OUTPUT);
    }

    bool execute(const Variant &args) override{
        int duration = args.get<int>("duration");
        changeWatering(duration);

        return true;
    }

    void update() override{
        bool on = !wateringTimer.finished();

//        Serial.print("update to: "); Serial.println(on);

        digitalWrite(pin, (on ? HIGH : LOW));
        setRunning(on);
    }

    void query(Variant & out) override{
        if(!running()){
            return;
        }

        out.set("prog", calcProgress());
    }

    void changeWatering(int duration){
        wateringTimer.start(duration);
        update();
    }

protected:
    float calcProgress() const{
        float progress =  1 - wateringTimer.timeToFinish()/(float)wateringTimer.time();
        return max(progress, 0.0f) * 100;
    }

protected:
    int pin;
    Timer wateringTimer;
};

#endif // SERVICES_H
