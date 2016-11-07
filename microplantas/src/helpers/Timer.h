#ifndef TIMER_H
#define TIMER_H

class Timer{
public:
    Timer(unsigned time=0): time(time)
    {}

    void start(unsigned timeMillis){
        this->time = timeMillis;
        start();
    }
    void start(){
        beginTimestamp = millis();
    }

    bool finished() const{
        return millis() - beginTimestamp >= time;
    }

protected:
    unsigned long beginTimestamp=0;
    unsigned time;
};

#endif // TIMER_H
