#include <XBee.h>

XBee xbee = XBee();

uint8_t payload[] = { 0, 0, 0 };

XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);

Tx64Request tx64 = Tx64Request(addr64, payload, sizeof(payload));

double xOff,yOff,zOff,xOut,yOut,zOut,accelMod;
boolean event,impact,steady;
volatile boolean fall;
float draft[300];  
int led,button,counter,interval1,interval2,draftPosition;
float sinVal;
int toneVal;

void setup() {
  // Calibração de offset
  xOff = 1.560;
  yOff = 1.615;
  zOff = 1.520;
 
  interval1 = 0;
  interval2 = 0;
  led = 1; // Define o estado inicial do LED como apagado
  
  int ledPin = 2;
  int buttonPin = 3;
  int buzzerPin = 8;
  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(2,1);
  
  attachInterrupt(1, controlButton, FALLING); // Chama funçao de interrupçao quando botao é acionado
  
  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() { 
  
  leAccel();
  
  if (accelMod < 0.20) // Possível movimento de queda 
  {
    event = true;
    if (counter < 50)
    {
      counter = 100; // Define total de registros do vetor modulo de aceleração 
    }
  }
  
  if (counter > 0 && event == true)
  {
    if (counter == 100) {
      draftPosition = 0; // Define posição inicial do vetor
    }
    draft[draftPosition] = accelMod;
    draftPosition++;
    counter--; 
    Serial.print(accelMod);
    Serial.print("   ");
    Serial.print(counter);
    Serial.println("   ");
  } 
  else if (counter == 0 && event == true) 
  {
    impact = false;
    for (int i = 1;i != 100;i++) {
      if (draft[i] > 4) { // Caso algum valor seja maior do que 2, tem-se um impacto.
        impact = true; 
      }
    }
    Serial.print("impact   ");
    Serial.print(impact);
    Serial.println("   ");
    
    steady = true;
    for (int i = 90;i != 100;i++) {
      if (draft[i] < 0.8 || draft[i] > 1.2) { // Caso algum dos ultimos valores esteja fora do limiar, o usuário não encontra-se parado. 
        steady = false;
      }
    }
    Serial.print("steady   ");
    Serial.print(steady);
    Serial.println("   ");
    
    if (impact == true && steady == true) { // Caso as duas condições sejam verdadeiras, caracteriza-se uma queda.
      fall = true;
    }    
    event = false;
    
  }
 
  if (fall == true)
  {
    envia(); // Chama função para enviar evento de queda via Xbee
    alarm(); // Chama função que aciona o buzzer com o som de alarme
    digitalWrite(2,led); 
    interval1++; 
    if (interval1 > 10 && led == 0) {
      led = 1;  
      interval1 = 0;
    } else if (interval1 > 10 && led == 1) {
      led = 0;
      interval1 = 0;
    }
  } else {
      led = 1;
      digitalWrite(2,led);
      noTone(8);
  }
}

void leAccel() { // Le dados Acelerometro
  
  xOut = 0;
  yOut = 0;
  zOut = 0;
  
  for (int i = 0; i < 3; i++) {
        xOut = xOut + ((analogRead(0)*4.79/1023 - xOff)/0.206); 
        yOut = yOut + ((analogRead(1)*4.79/1023 - yOff)/0.206);
        zOut = zOut + ((analogRead(2)*4.79/1023 - zOff)/0.206);
  } 
  
  xOut = xOut/3;
  yOut = yOut/3;
  zOut = zOut/3;
  
  /*Serial.print(xOut);
  Serial.print("   ");
  Serial.print(yOut);
  Serial.print("   ");
  Serial.print(zOut);
  Serial.print("   ");*/
  
  accelMod = sqrt(pow(xOut,2) + pow(yOut,2) + pow(zOut,2)); // Calcula o modulo da aceleração.  
  //Serial.print(accelMod);
  //Serial.println("   ");
}

void controlButton()
{
  fall = !fall;  
  
}

void envia()
{
  payload[0] = 0x00;
  payload[1] = 0x00;
  payload[2] = 0x00;
  xbee.send(tx64);
  delay(50); 
}

void alarm()
{  
  interval2++; 
  if (interval2 > 0 && interval2 < 10) 
  {
    int buzzerPin = 8;
    tone(buzzerPin, 880);
    //delay(5);
  } else if (interval2 >= 10 && interval2 < 20) {
    noTone(8);
    //delay(5);    
  } else {
     interval2 = 0; 
  }
}
