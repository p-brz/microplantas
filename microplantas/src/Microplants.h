#ifndef MICROPLANTS_H
#define MICROPLANTS_H

#include "Arduino.h"

#include "config.h"

#include "protocol.h"
#include "communication.h"
#include "sensors.h"
#include "services.h"
#include "services/ServicesHandler.h"

#include "helpers/Timer.h"

enum class roles : byte{
    sensor,
    bridge
};

class Microplants{
public:
    void setup() {
        Serial.begin(9600);

        //Pin to debug when standalone arduino is running
        pinMode(DEBUG_PIN, OUTPUT);
        digitalWrite(DEBUG_PIN, HIGH);

        this->role = getRole();

        bool isSensorNode = role == roles::sensor;

        uint64_t readPipe = (isSensorNode  ? 0    : pipe1);
        uint64_t writePipe = (isSensorNode ? pipe1 : 0);
        comm.begin(readPipe, writePipe);

        if(role == roles::sensor){
            setupSensors();

            servicesHandler.servManager.addService(1, &waterService1);
            servicesHandler.servManager.addService(2, &waterService2);
        }
    }

    void loop() {
        // put your main code here, to run repeatedly:
        if(role == roles::sensor){
            servicesHandler.handleServices();
//            sendSensors();
        }
        else{
            receiveSensors();
        }
    }

    void setupSensors(){
        pinMode(LIGHT_PIN, INPUT);
        pinMode(TEMP_PIN, INPUT);

        reader1.addSensor(TEMP_PIN, SensorType::temperature, readTemp2);
        reader2.addSensor(LIGHT_PIN, SensorType::light, readLight);
        reader2.addSensor(TEMP_PIN, SensorType::temperature, readTemp2);

        sensorAgregator.addNode(1, &reader1);
        sensorAgregator.addNode(2, &reader2);
    }

    void sendSensors(){
        if(poolingTimer.finished()){
            for(int i=0; i < sensorAgregator.count(); ++i){
//                Serial.print("Send data for reader: #"); Serial.println(i);
                sensorAgregator.sendData(i);
            }

            poolingTimer.start();
        }
    }

    void receiveSensors(){
        auto & receiver = comm.receiver;
        if ( receiver.available()){

            uint8_t len = receiver.read();

            // If a corrupt dynamic payload is received, it will be flushed
            if(!len){
                //      Serial.println("Got corrupt package (0 bytes)");
                return;
            }
            else{
                //      Serial.print("Received "); Serial.print(len); Serial.println(" bytes");
            }

            byte * data = receiver.data();

            //    Serial.print("Data: \"");
            for(int i=0; i < len; ++i){
                //      Serial.print(data[i - 1], HEX);
                Serial.print((char)data[i]);
            }
            //    Serial.println("\"");
        }

        //TODO: receber comandos da Serial e repassar via RF

        delay(30);
    }

protected:

    roles getRole(){
        Serial.println("ROLE: sensor(1), ponte(0)?");
        int r;

        while(!Serial.available()){
            delay(50);
        }
        r= Serial.read();


        roles role = ((r == '1') ? roles::sensor : roles::bridge);

        Serial.println((role == roles::sensor ? "sensor" : "bridge"));

        return role;
    }

private:
    roles role = roles::bridge;
    Timer poolingTimer{SENSOR_PERIOD};
    BasicSensorReader<2> reader1, reader2;

    WaterService waterService1{"regar", A0};
    WaterService waterService2{"regar", A1};

//    CommunicationRF comm{CE_PIN, CSN_PIN};
    CommunicationSerial comm;
    StaticJBuffer<JSON_BUFFER_SIZE> jsonBuffer;
    SensorAgregator<decltype(comm)> sensorAgregator{&comm, &jsonBuffer};
    ServicesHandler<decltype(comm)> servicesHandler{&comm, &jsonBuffer};
};

#endif // MICROPLANTS_H
