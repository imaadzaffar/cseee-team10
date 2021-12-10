const int motor = 9; 
int sensorState = A1;
long int duration=0;
long int RPM;
int HighestRPM= 6000;
int LowestRPM= 0;
int setRPM= 1800;
int outputValue;

void setup() {
  // set  the tran3istor pin as output:
  pinMode(motor, OUTPUT);
  pinMode(sensorState, INPUT);
  Serial.begin(9600);   
  outputValue = map(setRPM, LowestRPM, HighestRPM, 0, 128);
  analogWrite(motor, outputValue);
}
 
void loop() {
  duration = pulseIn(sensorState, HIGH);
  RPM=60000000/(duration*4);

  Serial.print("RPM=");
  Serial.println(RPM);

  if(RPM < setRPM - 100 && RPM > 100) {
    outputValue=outputValue +1;
    analogWrite(motor,outputValue);
    //Speed up the motor
  }

  if(RPM > setRPM + 100) {
    outputValue=outputValue -1;
    analogWrite(motor,outputValue);
    //Speed down the motor
  }

  if (RPM < setRPM + 100 && RPM > setRPM - 100) {
    //Make sure the motor maintains this value
  }

  Serial.print("outputvalue=");
  Serial.println(outputValue);
}
  
