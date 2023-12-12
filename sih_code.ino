#include <Wire.h>
#include<SoftwareSerial.h>
SoftwareSerial BTSerial(0,1);//RX,TX
long accelX, accelY, accelZ;
int x = 0;
int LastTime = 0;
bool BPMTiming = false;
bool BeatComplete = false;
int BPM = 0;    
#define UpperThreshold 518
#define LowerThreshold 490    




void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  Wire.begin();
  Serial.println("CLEARDATA");
  Serial.println("LABEL,Time,Ax,Ay,Az,ECG,BPM,");
  Serial.println("RESETTIMER");
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(13,OUTPUT);
  setupMPU();
}


void loop() {
  //accelerometer
  accelerometer();
  //ecg
  int ecg=analogRead(A1);
  //pulse
  pulse();
  //bluetooth
  alert();
  
  
  
  Serial.print("DATA,TIME,");
  Serial.print(accelX),Serial.print(","),Serial.print(accelY),Serial.print(","),Serial.print(accelZ),Serial.print(","),Serial.print(ecg),Serial.print(","),Serial.print(BPM),Serial.print(","),Serial.println();
  delay(100);
}





void setupMPU()
{ 
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  

  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

 void accelerometer()
  {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read();//Store last two bytes into accelZ

  accelX=accelX/2048;
  accelY=accelY/2048;
  accelZ=accelZ/2048;
  }
void pulse()
 {
  int value = analogRead(A0);
  if (value > UpperThreshold) {
    if (BeatComplete) {
      BPM = millis() - LastTime;
      BPM = int(60 / (float(BPM) / 1000));
      BPMTiming = false;
      BeatComplete = false;
    }
    if (BPMTiming == false) {
      LastTime = millis();
      BPMTiming = true;
    }
  }
  if ((value < LowerThreshold) & (BPMTiming))
    BeatComplete = true;
    // display bpm

  x++;
  /*Signal = analogRead(A0); // Read the PulseSensor's value.
  // Assign this value to the "Signal" variable.
  if (Signal > UpperThreshold) {
    // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW); //  Else, the signal must be below "550", so "turn-off" this LED.
  }*/
 } 
  
void alert()
{
  int a=analogRead(A1);
  if (a>50)
  {
    digitalWrite(13,1);
    BTSerial.println("*****ALERT : ECG IS HIGH*****");
    }
  else if (a<50)
  {
    digitalWrite(13,1);
    BTSerial.println("*****ALERT : ECG IS LOW*****");
}
  else 
   digitalWrite(13,0);
 }
