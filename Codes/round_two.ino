#include <Servo.h>
#include <Wire.h>
#include <MPU6050_light.h>
//#include "HUSKYLENS.h"
//#include "SoftwareSerial.h"

//SoftwareSerial huskySerial(D6, D5);

#define m1 8
#define m2 9
#define trigPin1 4
#define echoPin1 5
#define trigPin2 6
#define echoPin2 7

struct Data {
  int16_t x;
  int16_t y;
  int16_t z;
};

MPU6050 mpu(Wire);
int16_t ax, ay, az;
int16_t gx, gy, gz;
float pitch, roll, yaw;

Servo servo;
Data gData;
//HUSKYLENS huskylens;

int spos = 100;  // max = 140, min = 60
long distance_1 = 0;
long distance_2 = 0;
bool stopped = false;

void setup(){
  Serial.begin(115200);
  //huskySerial.begin(9600);
  Wire.begin();

  if (mpu.begin() != 0) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }
  mpu.calcOffsets();

  // while (!huskylens.begin(huskySerial)){
  //   Serial.println("Husky Lens initialization failed!");
  //   delay(100);
  // }

  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  servo.attach(11);
}

void loop()
{
  updateRotation();
  updateDistance();
  //camera();

  if (gData.z > -360 && gData.z < 360 && !stopped) forward();
  else stop();
}

// void camera()
// {
//   if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
//     else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
//     else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
//     else
//     {
//         Serial.println(F("###########"));
//         while (huskylens.available())
//         {
//             HUSKYLENSResult result = huskylens.read();
//             printResult(result);
//         }    
//     }
// }

// void printResult(HUSKYLENSResult result){
//     if (result.command == COMMAND_RETURN_BLOCK){
//         Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
//     }
//     else if (result.command == COMMAND_RETURN_ARROW){
//         Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
//     }
//     else{
//         Serial.println("Object unknown!");
//     }
// }

void updateRotation()
{
  mpu.update();
  gData.x = mpu.getAngleX();
  gData.y = mpu.getAngleY();
  gData.z = mpu.getAngleZ();

  Serial.print("Z-Axis: ");
  Serial.println(gData.z);
}

void updateDistance()
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  
  long duration_1 = pulseIn(echoPin1, HIGH);
  distance_1 = duration_1 * 0.034 / 2;

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  long duration_2 = pulseIn(echoPin2, HIGH);
  distance_2 = duration_2 * 0.034 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance_2);
  Serial.println(" cm");

  Serial.print("Distance: ");
  Serial.print(distance_1);
  Serial.println(" cm");
  
  if(distance_1 > distance_2){
    spos = map(distance_2, 0, 50, 60, 100);
    spos = constrain(spos, 60, 100);
  }
  if(distance_1 < distance_2){
    spos = map(distance_1, 0, 50, 140, 100);
    spos = constrain(spos, 100, 140);
  }

  spos = constrain(spos, 60, 140);
  servo.write(spos);
  delay(1);
}

void forward()
{
  digitalWrite(m1, HIGH);
  digitalWrite(m2, LOW);
}

void backward()
{
  digitalWrite(m1, LOW);
  digitalWrite(m2, HIGH);
}

void stop()
{
  digitalWrite(m1, LOW);
  digitalWrite(m2, LOW);
  stopped = true;
}
