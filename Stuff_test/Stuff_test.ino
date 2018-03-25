#include <dummy.h>

//This tests devices.

int A = A0;
int motion = 16;
void setup() {
  // put your setup code here, to run once:
  pinMode (A, INPUT);
  pinMode (motion, INPUT);
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  double temp = getTemp();
  boolean moved = checkMotion();
  String motion = "";
  if(moved)
  {
    motion = "Positive";
  }
  else
  {
    motion = "Negative";
  }
  
//  Serial.println("The temperature is ");
//  Serial.println (temp);
//  Serial.println("Motion Detect:");
//  Serial.println(checkMotion());

  // log message
  Serial.println(String("T: ") + temp + String("; Motion: ") + motion);
  
  delay(100);
}

double getTemp()
{
  int reading = analogRead(A);
  Serial.println(reading);
  double temp = (reading - 500)/10.0;
  return temp;
}

boolean checkMotion()
{
  if(digitalRead(motion) == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

