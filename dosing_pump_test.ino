// Motor B (Dosing Pump) - L298N
#define ENB 12    // PWM-enabled pin
#define IN3 10
#define IN4 11


void setup() {
  // Setup motor control pins
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.begin(9600);
  delay(2000);  // Wait before starting
}

void loop() {
  // Run pump forward
  Serial.println("Dosing pump ON...");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 180);  // speed from 0–255
  // Stop pump
  //Serial.println("Dosing pump OFF...");
  //analogWrite(ENB, 0);     // Stop motor

  //digitalWrite(IN3, LOW);
  //digitalWrite(IN4, LOW);
  //digitalWrite(IN1, LOW);
  //digitalWrite(IN2, LOW);
  //analogWrite(ENA, 255);
  //delay(2000);  // Wait for 5 seconds before next cycle"
}
