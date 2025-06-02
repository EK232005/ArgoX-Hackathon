//TDS sensor
#include <EEPROM.h>
#include "GravityTDS.h"
#define TdsSensorPin A1
GravityTDS gravityTds;
float temperature = 25, tdsValue = 0;

//pH sensor
#define phsensorpin A0  //pH meter Analog output to Arduino Analog Input 0
float calibration_value = 21.34 + 0.7;
unsigned long int avgValue;  //Store the average value of the sensor feedback
int buf[10], phtemp;
float phValue;

//Turbidity sensor
#define turbsensorpin A2  // Connect turbidity sensor to Digital Pin 2
int turbinput;
float turbval[10], turbtemp, avgturbvoltage, turbvalout;
int modevar = 1;

//Dosing and Flushing 
#define FLUSH_PIN 3     // Example motor driver pin for flush
#define DOSE1_PIN 9     // Fertilizer 1 PWM pin
#define DOSE2_PIN 11     // Fertilizer 2 PWM pin
#define IN1 7
#define IN2 8
#define IN3 10
#define IN4 11

void flushSystem() {
  analogWrite(FLUSH_PIN, 180);
  delay(3000); // 3 seconds flushing
  analogWrite(FLUSH_PIN, 0);
}

unsigned long lastDoseTime = 0;
const unsigned long doseInterval = 10000;  // 4 minutes = 240,000 ms


void setup() {
  Serial.begin(9600);
  //pH setup
  pinMode(phsensorpin, INPUT);
  //TDS setup
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);       //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();            //initialization
  //Turbidity setup
  pinMode(turbsensorpin, INPUT);  //Set the turbidity sensor pin to input mode
  //dosing pump setup
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
}
void loop() {

  switch (modevar) {
    case 1:
      gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
      gravityTds.update();                     //sample and calculate
      tdsValue = gravityTds.getTdsValue();     // then get the value
      modevar++;
      break;

    case 2:
      for (int i = 0; i < 10; i++) {  //Get 10 sample value from the sensor for smooth the value
        buf[i] = analogRead(phsensorpin);
        delay(10);
      }
      for (int i = 0; i < 9; i++) {  //sort the analog from small to large
        for (int j = i + 1; j < 10; j++) {
          if (buf[i] > buf[j]) {
            phtemp = buf[i];
            buf[i] = buf[j];
            buf[j] = phtemp;
          }
        }
      }
      avgValue = 0;
      for (int i = 2; i < 8; i++) avgValue += buf[i];  //take the average value of 6 center sample
      phValue = (float)avgValue * 5.0 / 1024 / 6;      //convert the analog into millivolt
      phValue = -5.70 * phValue + calibration_value;   //convert the millivolt into pH value
      modevar++;
      break;

    case 3:
      for (int i = 0; i < 10; i++) {            //Get 10 sample value from the sensor for smooth the value
        turbinput = analogRead(turbsensorpin);  // read the input on analog input:
        turbval[i] = turbinput;
        delay(10);
      }
      for (int i = 0; i < 9; i++) {  //sort the analog from small to large
        for (int j = i + 1; j < 10; j++) {
          if (turbval[i] > turbval[j]) {
            turbtemp = turbval[i];
            turbval[i] = turbval[j];
            turbval[j] = turbtemp;
          }
        }
      }
      avgturbvoltage = 0;
      for (int i = 2; i < 8; i++) avgturbvoltage += turbval[i];  //take the average value of 6 center sample
      avgturbvoltage = (float)avgturbvoltage * 5.0 / 1024 / 6;   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
      turbvalout = (-1120.4 * pow(avgturbvoltage, 2)) + (5742.3 * avgturbvoltage) - 4352.9;
      turbvalout = constrain(turbvalout, 0, 3000);
      modevar++;
      break;

    default:
      modevar = 1;
      float ecValue = tdsValue / 640.0;
      // Send sensor data as: SENSOR:tds,ph,turbidity
      Serial.print("SENSOR:");
      Serial.print(tdsValue, 0);       // TDS as integer
      Serial.print(",");
      Serial.print(phValue, 2);        // pH with 2 decimal places
      Serial.print(",");
      Serial.println(turbvalout, 1);   // Turbidity with 1 decimal
      delay(1000);
      break;
  };
  
  unsigned long currentTime = millis();
  if (currentTime - lastDoseTime >= doseInterval) {
    Serial.println("Dosing pump ON...");
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(DOSE1_PIN, 180);  // or doseFertilizer2() or both
    Serial.println("Dosing Fertilizer 1");
    lastDoseTime = currentTime;
  }

}
