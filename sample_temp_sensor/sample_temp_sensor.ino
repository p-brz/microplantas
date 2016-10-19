//Programa : Sensor de temperatura LM35
//Arduino & Cia

int pin = 0; // Pino analogico para ligacao do LM35

// Variaveis que armazenam a temperatura em Celsius e Fahrenheit
int tempc = 0, tempf = 0;
int samples[8]; // Array para precisão na medição

// Variáveis que guardam a temperatura máxima e mínima
int maxtemp = -100, mintemp = 100;
int i;

void readTemp();
void readLight();

void setup(){
  Serial.begin(9600);     // Inicializa comunicação serial

  pinMode(A2, INPUT);
}

void loop()
{
  readTemp();
  readLight();
  delay(1000); // Aguarda 1 segundo e reinicia o processo
}


void readTemp(){
  for (i = 0; i <= 7; i++) { // Loop que faz a leitura da temperatura 8 vezes
    samples[i] = ( 5.0 * analogRead(pin) * 100.0) / 1024.0;
    //A cada leitura, incrementa o valor da variavel tempc
    tempc = tempc + samples[i];
    delay(100);
  }

  // Divide a variavel tempc por 8, para obter precisão na medição
  tempc = tempc / 8.0;
  //Converte a temperatura em Fahrenheit e armazena na variável tempf
  tempf = (tempc * 9) / 5 + 32;
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
  Serial.print(tempf, DEC);
  Serial.print(" Fahr. -> ");
  Serial.print(" Min : ");
  Serial.print(mintemp, DEC);
  Serial.print("  Max: ");
  Serial.println(maxtemp, DEC);

  tempc = 0;
}

void readLight(){
  int value = analogRead(A2);
  Serial.print("Luz: ");
  Serial.println(value);
}

