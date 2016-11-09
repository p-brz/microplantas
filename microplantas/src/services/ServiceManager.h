#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "Service.h"
#include <string.h>

class ServiceManager{
    struct Pair{
        int nodeId = 0;
        Service * serv = nullptr;
    };

public:
    bool addService(int nodeId, Service * s){
        if(numServices >= 5){
            return false;
        }

        Pair p;
        p.nodeId = nodeId;
        p.serv = s;
        services[numServices] = p;
        ++numServices;

        s->setup();

        return true;
    }

    Service * getService(int nodeId, const char * servName){
        for (int i = 0; i < numServices; ++i) {
            if(services[i].nodeId == nodeId){
                Service * s = services[i].serv;
                if(strcmp(s->name, servName) == 0){
                    return s;
                }
            }
        }
        return nullptr;
    }

    Pair services[5];
    int numServices = 0;
};

#endif // SERVICEMANAGER_H
