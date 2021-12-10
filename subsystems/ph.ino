void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(8,OUTPUT);
  TargetpHValue = 5
 

Serial.begin(9600);
}

void loop() {
  int pHSensorValue = analogRead(A2);
  float pHOutput = map(pHSensorValue,0,1023,0,14);
  pHDigitalDifference = pHOutput-TargetpHValue;
  analogWrite(6,pHOutput);
 
  if （pHDigitalDifference > 0.5）{
    digitalWrite(10,HIGH);
    delay(1000);
    digitalWrite(10,LOW);
  }
  else if （pHDigitalDifference < -0.5）{
    digitalWrite(11,HIGH);
    delay(1000);
    digitalWrite(11,LOW);
  }
}
