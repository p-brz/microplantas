#ifndef TIMER_H
#define TIMER_H

class Timer{
public:
    Timer(unsigned time=0): _time(time)
    {}

    void start(unsigned timeMillis){
        this->_time = timeMillis;
        start();
    }
    void start(){
        beginTimestamp = millis();
    }

    bool finished() const{
        return millis() - beginTimestamp >= _time;
    }

    unsigned timeToFinish() const{
        auto diff = millis() - beginTimestamp;
        return _time - diff;
    }

    unsigned time() const{
        return _time;
    }

protected:
    unsigned long beginTimestamp=0;
    unsigned _time;
};

#endif // TIMER_H
