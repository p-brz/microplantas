#ifndef SENSORS_H
#define SENSORS_H

#include "sensors/SensorAgregator.h"
#include "sensors/BasicSensorReader.h"

int readTemp(byte pin){
    static int maxtemp = -100, mintemp = 100;

    int tempc = 0;
    int samples[8];

    for (int i = 0; i <= 7; i++) { // Loop que faz a leitura da temperatura 8 vezes
        samples[i] = ( 5.0 * analogRead(pin) * 100.0) / 1024.0;
        //A cada leitura, incrementa o valor da variavel tempc
        tempc = tempc + samples[i];
        delay(100);
    }

    // Divide a variavel tempc por 8, para obter precisão na medição
    tempc = tempc / 8.0;
    //Converte a temperatura em Fahrenheit e armazena na variável tempf
    //  tempf = (tempc * 9) / 5 + 32;
    //Armazena a temperatura máxima na variável maxtemp
    if (tempc > maxtemp) {
        maxtemp = tempc;
    }
    //Armazena a temperatura máxima na vari[avel mintemp
    if (tempc < mintemp) {
        mintemp = tempc;
    }

    // As linhas abaixo enviam para o monitor serial a temperatura em
    // Celsius e Fahrenheit,
    // e também as temperaturas máxima e mínima registradas

    Serial.print(tempc, DEC);
    Serial.print(" Cels., ");
    //  Serial.print(tempf, DEC);
    //  Serial.print(" Fahr. -> ");
    Serial.print(" Min : ");
    Serial.print(mintemp, DEC);
    Serial.print("  Max: ");
    Serial.println(maxtemp, DEC);

    return tempc;
}

int readTemp2(byte pin){
    double temperatura = (analogRead(pin) * 0.00488);
    temperatura = temperatura * 100;

    return (int)(temperatura);
}

int readLight(byte pin){
    int value = analogRead(pin);
    return (value * 100)/1024;
}

#endif // SENSORS_H
