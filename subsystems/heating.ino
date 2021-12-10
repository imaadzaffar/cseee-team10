float target = 5.5;
int R1 = 1000;
float R2,T,V;
int Vcc = 5;
#define PIN_SENSOR A3
#define PIN_SWITCH 9
void setup() {
  // put your setup code here, to run once:
   pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_SWITCH, OUTPUT);
Serial.begin(9600);
}


void loop() {
 Serial.println(R2);
float V=analogRead(PIN_SENSOR);
R2= R1*Vcc/(1023-V);  


  // Temperature read
  float c = R2;
  Serial.print("Temperature: ");
  Serial.println(c);
  // Regulation
  if (c > target)
  {
    digitalWrite(PIN_SWITCH, LOW);
    Serial.print("\tHeater OFF");
  } else {
    digitalWrite(PIN_SWITCH, HIGH);
    Serial.print("\tHeater ON");
  }
  delay(2000);
}
