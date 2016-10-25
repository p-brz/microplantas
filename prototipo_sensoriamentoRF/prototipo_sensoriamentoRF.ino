/* NRF24
    Uno   Mega
CE    9   41     (opcional)
CSN   8  40    (opcional)
SCK   13  52
MOSI  11  51
MISO  12  50

*/

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega, Mega ADK
#define CE_PIN 41
#define CSN_PIN 40
#else
#define CE_PIN 9
#define CSN_PIN 8
#endif

#include "protocol.h"
#include "rf_communication.h"
#include "SensorAgregator.h"

/** Constants **/
constexpr const int LIGHT_PIN = A0;
constexpr const int TEMP_PIN = A1;
constexpr const uint64_t pipe = 0xE13CBAF433LL;
constexpr const int SENSOR_PERIOD = 1000;

/** Variables **/
Communication comm{CE_PIN, CSN_PIN};
BasicSensorReader<2> reader1, reader2;
SensorAgregator<> sensorAgregator{&comm};
bool emissor = false;

/** Prototypes **/
int  getRole();
void setupSensors();
void sendSensors();
void receiveSensors();

int readTemperature(int pin);
int readLight(int pin);

void setup() {
  Serial.begin(9600);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

//  emissor = (getRole() == 1);
  
//  Serial.print("is emitter: "); Serial.println(emissor);
  emissor = false;
  
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

  reader1.addSensor(LIGHT_PIN, SensorType::light, readLight);
  reader1.addSensor(TEMP_PIN, SensorType::temperature, readTemp2);
  reader2.addSensor(TEMP_PIN, SensorType::temperature, readTemp2);

  sensorAgregator.addNode(1, &reader1);
  sensorAgregator.addNode(2, &reader2);
  sensorAgregator.addNode(1, &reader1);
}

void sendSensors(){
//    int value = analogRead(SENSOR_PIN);
//    Serial.print("analog read: "); Serial.println(value);
//    
////    bool sent = sendBytes(&value, sizeof(value));
//
//    comm.sender.print("value: "); 
//    comm.sender.print(value);
//    int sBytes = comm.sender.send();
//    Serial.print("sent ");Serial.print(sBytes); Serial.println(" bytes");

    for(int i=0; i < sensorAgregator.count(); ++i){
      Serial.print("Send data for reader: "); Serial.println(i);
      sensorAgregator.sendData(i);  
    }
    
    
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


#define SAMPLE_TIME 100
//int readTemperature(int pin){
//  int tempc = 0;
//  
//  for (int i = 0; i < 8; i++) { // Loop que faz a leitura da temperatura 8 vezes
//    int sample = ( 5.0 * analogRead(pin) * 100.0) / 1024.0;
//    //A cada leitura, incrementa o valor da variavel tempc
//    tempc = tempc + sample;
//    delay(SAMPLE_TIME);
//  }
//
//  // Divide a variavel tempc por 8, para obter precisão na medição
//  tempc = tempc / 8.0;
//
//  Serial.print("Read temp: "); Serial.print(tempc);
//  Serial.print(" from pin: "); Serial.print(pin);
//  Serial.print("; raw: "); Serial.println(analogRead(pin));
//
//  return tempc;
//}

int maxtemp = -100, mintemp = 100;
int readTemp(int pin){
  int tempc = 0;
  int samples[8];
  
  for (int i = 0; i <= 7; i++) { // Loop que faz a leitura da temperatura 8 vezes
    samples[i] = ( 5.0 * analogRead(TEMP_PIN) * 100.0) / 1024.0;
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

int readTemp2(int pin){
    double temperatura = (analogRead(pin) * 0.00488);
    temperatura = temperatura * 100;

    return (int)(temperatura);
}

int readLight(int pin){
  int value = analogRead(pin);
  return (value * 100)/1024;
}


