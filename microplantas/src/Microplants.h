#ifndef MICROPLANTS_H
#define MICROPLANTS_H

#include "Arduino.h"

#include "config.h"

#include "protocol.h"
#include "communication.h"
#include "sensors.h"

class Microplants{
public:
    void setup() {
        Serial.begin(9600);

        pinMode(7, OUTPUT);
        digitalWrite(7, HIGH);

        emissor = (getRole() == 1);

        Serial.print("is emitter: "); Serial.println(emissor);
        //  emissor = false;

        uint64_t readPipe = (emissor ? 0 : pipe);
        uint64_t writePipe = (emissor ? pipe : 0);
        comm.begin(readPipe, writePipe);

        if(emissor){
            setupSensors();
        }
    }

    void loop() {
        // put your main code here, to run repeatedly:
        if(emissor){
            handleServices();
            sendSensors();
        }
        else{
            receiveSensors();
        }
    }

    int getRole(){
        Serial.println("ROLE: emissor(1), receptor(0)?");
        int r;
        while(!Serial.available()){
            delay(50);
        }
        r= Serial.read();

        return (r == '1') ? 1 : 0;
    }

    void setupSensors(){
        pinMode(LIGHT_PIN, INPUT);
        pinMode(TEMP_PIN, INPUT);

        reader1.addSensor(TEMP_PIN, SensorType::temperature, readTemp2);
        reader2.addSensor(LIGHT_PIN, SensorType::light, readLight);
        reader2.addSensor(TEMP_PIN, SensorType::temperature, readTemp2);

        sensorAgregator.addNode(1, &reader1);
        sensorAgregator.addNode(2, &reader2);
        //  sensorAgregator.addNode(1, &reader1);
    }

    void handleServices(){
        //TODO: implement
    }

    void sendSensors(){
        for(int i=0; i < sensorAgregator.count(); ++i){
            Serial.print("Send data for reader: #"); Serial.println(i);
            sensorAgregator.sendData(i);

            delay(10);
        }

        //TODO: remover delay!!!!! (para não perder evento)
        // Try again in a short while
        delay(SENSOR_PERIOD);
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
        delay(30);
    }

private:
    /** Variables **/
    CommunicationRF comm{CE_PIN, CSN_PIN};
//    CommunicationSerial comm;
    BasicSensorReader<2> reader1, reader2;
    SensorAgregator<decltype(comm)> sensorAgregator{&comm};
    bool emissor = false;

};

#endif // MICROPLANTS_H
