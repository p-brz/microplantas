#ifndef CONFIG_H
#define CONFIG_H

/* NRF24
    Uno   Mega
CSN   8   40    (variável - definido em CSN_PIN)
CE    9   41    (variável - definido em CE_PIN)
MOSI  11  51
MISO  12  50
SCK   13  52

*/

#include "Arduino.h"

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega, Mega ADK
#define CE_PIN 41
#define CSN_PIN 40
#else
#define CE_PIN 9
#define CSN_PIN 8
#endif

/** Constants **/
constexpr const int LIGHT_PIN = A0;
constexpr const int TEMP_PIN = A1;
constexpr const int DEBUG_PIN = 7;
constexpr const uint64_t pipe1 = 0xE13CBAF433LL;
constexpr const int SENSOR_PERIOD = 2000;

#endif // CONFIG_H
