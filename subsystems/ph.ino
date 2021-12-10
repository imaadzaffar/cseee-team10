const int LowPHpump= 9;
const int HighPHpump=10;

float TargetPH=5;

void setup()
{
  pinMode(LowPHpump, OUTPUT);
  pinMode(HighPHpump, OUTPUT);
 
 
Serial.begin(9600);
delay(300);
}


void loop() {
 
  float Analoguevalue=analogRead(A2);

  float RealpH=map(Analoguevalue,540,465,4,10);
   

  if (Analoguevalue>465 && Analoguevalue<480)
  {
    Serial.println((String)"Solution pH is 10 "+"Analogue value ="+Analoguevalue);
  }
  else if (Analoguevalue>495 && Analoguevalue<500)
  {
    Serial.println((String)"Solution pH is 7 "+"Analogue value ="+Analoguevalue);  
  }
    else if (Analoguevalue>530 && Analoguevalue<540)
  {
   Serial.println((String)"Solution pH is 4 "+"Analogue value ="+Analoguevalue);
  }

  if (RealpH<TargetPH)
  {
    digitalWrite(LowPHpump, HIGH);
    digitalWrite(HighPHpump, LOW);
  }
  else if (RealpH>TargetPH)
  {
    digitalWrite(LowPHpump, HIGH);
    digitalWrite(HighPHpump, LOW);
  }
  else
  {
    digitalWrite(LowPHpump, LOW);
    digitalWrite(HighPHpump, LOW);
   
  }
 
Serial.println((String)"Analogue value ="+Analoguevalue);  
Serial.println((String)"Solution pH is ="+RealpH);
     

}
