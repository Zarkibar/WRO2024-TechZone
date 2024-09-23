#include <Servo.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include "HUSKYLENS.h"
#include "Car_PID.h"

#define m1 D3
#define m2 D0
#define trigPin1 D7
#define echoPin1 D8
#define trigPin2 D6
#define echoPin2 D5
#define servoPin D4

float Kp = 1.0;
float Kd = 1.0;
float Ki = 1.0;
float MAX_OUTPUT = 90.0;
float MIN_OUTPUT = -90.0;

struct Data {
  int16_t x;
  int16_t y;
  int16_t z;
};

Servo servo;
Data gData;
HUSKYLENS huskylens;
MPU6050 mpu(Wire);
PID pid(Kp, Ki, Kd, MAX_OUTPUT, MIN_OUTPUT);

int16_t ax, ay, az;
int16_t gx, gy, gz;
float pitch, roll, yaw;

int objWidth[8] = {0};
int objDist[8] = {0};
int8_t objType[8] = {0};     // -1 for Red, 0 for Null, 1 for Green
int minDist = 0;
int8_t minType = 0;          // -1 for Red, 0 for Null, 1 for Green

int spos = 100;  // max = 140, min = 60
long distance_1 = 0;
long distance_2 = 0;
bool stopped = false;
bool objectDetected = false;
int previousTime = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();

  if (mpu.begin() != 0) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }
  mpu.calcOffsets();

  while (!huskylens.begin(Serial))
  {
        Serial.println("Nawh bro");
        delay(100);
  }

  while (!huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION)) {
    Serial.println("Failed to set Color Recognition Algorithm!");
  }

  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  servo.attach(servoPin);
}

void loop()
{
  if (gData.z > -720 && gData.z < 720 && !stopped) {
    forward();

    //camera();
    updateRotation();
    handleCrashing();
    updateDistance();
  }
  else stop();
}

void camera()
{
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else
  {
    Serial.println(F("###########"));
    if (huskylens.available())
    {
      for (int i = 0; i < huskylens.available(); i++)
      {
        HUSKYLENSResult result = huskylens.read();

        objWidth[i] = result.width;
        if (objWidth[i] == 0) 
          objDist[i] = 0;
        else 
          objDist[i] = 1725/objWidth[i];

        if (result.ID == 1) 
          objType[i] = -1;
        else if (result.ID == 2) 
          objType[i] = 1;
        else 
          objType[i] = 0;
      }

      for (int i = 0; i < huskylens.available(); i++){
        if (objDist[i] < minDist){
          minDist = objDist[i];
          minType = objType[i];
        }
      }

      if (minType == -1){       // IF RED
        objectDetected = true;
        spos = map(minDist, 15, 0, 100, 150);
      }
      else if (minType == 1){  // IF GREEN
        objectDetected = true;
        spos = map(minDist, 15, 0, 100, 50);
      }

    } else {
      objectDetected = false;
    }
  }
}

void handleCrashing()
{
  // IF NOT MOVING,
  // -GO BACKWARDS FOR COUPLE OF SECONDS
  // -SERVO WILL MOVE ACCORDING TO THE gData.z ROTATION
}

void updateRotation()
{
  mpu.update();
  gData.x = mpu.getAngleX();
  gData.y = mpu.getAngleY();
  gData.z = mpu.getAngleZ();

  //Serial.print("Z-Axis: ");
  //Serial.println(gData.z);
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

  // // Print the distance to the serial monitor
  // Serial.print("Distance 1: ");
  // Serial.print(distance_2);
  // Serial.print(" cm    ");

  // Serial.print("Distance 2: ");
  // Serial.print(distance_1);
  // Serial.println(" cm");

  // if (true){
  //   if (distance_2 > 145){
  //     spos = 50;
  //   }
  //   else if (distance_1 > 145){
  //     spos = 150;
  //   }
  //   else if((distance_2-distance_1) > 1){
  //     spos = map(distance_1, 0, 50, 50, 100);
  //     spos = constrain(spos, 50, 100);
  //   }
  //   else if((distance_2-distance_1) < -1){
  //     spos = map(distance_2, 0, 50, 150, 100);
  //     spos = constrain(spos, 100, 150);
  //   }
  //   else {
  //     spos = 100;
  //   }
  // }

  if ((millis() - previousTime) >= 100){
    float dt = (millis() - previousTime)/1000;
    int angle = pid.compute(distance_2 - distance_1, dt);
    spos = map(angle, MIN_OUTPUT, MAX_OUTPUT, 150, 50);
    previousTime = millis();
  }

  spos = constrain(spos, 50, 150);
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
  servo.write(100);
  stopped = true;
}
