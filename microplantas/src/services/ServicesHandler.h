#ifndef SERVICESHANDLER_H
#define SERVICESHANDLER_H

#include "config.h"
#include "ServiceManager.h"
#include "helpers/Timer.h"

#include "serialization/ClearableBuffer.h"

enum class StatusCode : int{
    Ok              = 200,
    InvalidRequest  = 400,
    NotFound        = 404,
    ServerError     = 500
};

template<typename Communication>
class ServicesHandler{
public:
    ServicesHandler(Communication * comm, ClearableBuffer * buffer)
        : comm(comm)
        , jsonBuffer(buffer)
    {}

    void servicesUpdate()
    {
        bool display = timer.finished();

        if(display){
            timer.start();
        }

        for (int i = 0; i < servManager.numServices; ++i) {
            Service * s = servManager.services[i].serv;
            int nodeId = servManager.services[i].nodeId;

            if(s->running()){
                s->update();

                if(display || !s->running()){
                    jsonBuffer->clear();
                    JsonObject & obj = jsonBuffer->createObject();
                    obj.set(id, nodeId);
                    obj.set(event, "notify");

                    JsonObject & child = obj.createNestedObject(s->name);

                    s->query(child);

                    comm->sendJson(obj);
                }
            }
        }

        jsonBuffer->clear();
    }

    void handleServices(){
        //TODO: implement
        /*
         * 1. Tem dados para receber?
         *  1.1. Lê dados em buffer
         *  1.2. Identifica o comando e argumentos
         *  1.3. Executa serviço
         *
         * 2. Atualiza cada serviço em execução:
         *  2.1. Se serviço sofrer atualização
         *  2.2. Envia evento de notificação
         *
        */


        jsonBuffer->clear();

        if(comm->available()){
            Serial.println("handleService: communication available");
            handleRequest(*jsonBuffer);
        }

        servicesUpdate();
    }

    void sendStatus(StatusCode status){
        StaticJBuffer<30> tmpBuffer;
        JsonObject & obj = tmpBuffer.createObject();
        obj.set("res", (int)status);

        comm->sendJson(obj);
    }

protected:

    void handleRequest(ClearableBuffer & buffer){
        JsonObject & obj = comm->readJsonObject(&buffer);

        if(!obj.success()){
            sendStatus(StatusCode::InvalidRequest);
            return;
        }

        int nodeId = obj.get<int>(id);
        const char * cmd = obj.get<const char *>(command);
        Service * s = servManager.getService(nodeId, cmd);

        if(s){
//            Serial.print("node: "); Serial.print(nodeId);
//            Serial.print("; cmd: ");Serial.println(cmd);

            if(s->execute(obj)){
                sendStatus(StatusCode::Ok);
            }
            else{
                sendStatus(StatusCode::ServerError);
            }
        }
        else{
            sendStatus(StatusCode::NotFound);
        }
    }

protected:
    Communication * comm;
    ClearableBuffer * jsonBuffer;
    Timer timer{450};

public:
    ServiceManager servManager;
    constexpr static const char * id = "id";
    constexpr static const char * command = "cmd";
    constexpr static const char * event = "evt";
};

#endif // SERVICESHANDLER_H
