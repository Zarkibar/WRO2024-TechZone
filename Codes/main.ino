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
#define buttonPin A0

// For PID
float Kp = 0.7;                // Proportional Constant
float Kd = 0.001;              // Derivative Constant
float Ki = 0.01;               // Integral Constant
float MAX_OUTPUT = 100.0;      // Maximum output of PID
float MIN_OUTPUT = -100.0;     // Minimum output of PID
int SERVO_MIN = 48;            // Minimum servo angle
int SERVO_MAX = 148;           // Maximum servo angle

struct Data { 
  int16_t x;
  int16_t y;
  int16_t z;
};

Servo servo;
Data gData;                   // Gyroscope Data
Data aData;                   // Accelerometer Data
HUSKYLENS huskylens;
MPU6050 mpu(Wire);
PID pid(Kp, Ki, Kd, MAX_OUTPUT, MIN_OUTPUT);

int minDist = 999999;      // Obstact distance with the minimum value
int8_t minID = 0;          // -1 for Red, 0 for Null, 1 for Green

int spos = 102;
long distance_1 = 0;
long distance_2 = 0;
bool stopped = false;
bool shouldStart = false;
bool objectDetected = false;
int objValue = 0;             // Helps to move the track to right or left for PID (for 2nd round)
int angle = 0;
int previousTime = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();

  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(buttonPin, INPUT);
  servo.attach(servoPin);

  if (mpu.begin() != 0) {
    //Serial.println("MPU6050 initialization failed!");
    while (1);
  }
  mpu.calcOffsets();

  while (!huskylens.begin(Serial))
  {
    //error_message = "Nawh bro";
    delay(100);
  }

  while (!huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION)) {
    //error_message = "Failed to set Color Recognition Algorithm!";
  }
}

void loop()
{
  if (analogRead(buttonPin) > 200) shouldStart = true;
  if (shouldStart != true) return;

  updateRotation();      // Updates orientation with Gyro

  if (gData.z > -1070 && gData.z < 1070 && !stopped) 
  {
    forward();            // Moves forward

    camera();             // Manages camera information
    handleCrashing();     // If crashes then reposition automatically
    updateDistance();     // Tries to keep itself in the center of the walls
  }
  else stop();
}

void camera()
{
  objValue = 0;

  if (huskylens.request())
  {
    while (huskylens.available())
    {
      HUSKYLENSResult result = huskylens.read();

      if (result.ID == 1){
        if ((1725/result.width) < minDist){
          objValue = 50;
          minDist = 1725/result.width;
        }
      }
      else if (result.ID == 2){
        if ((1725/result.width) < minDist){
          objValue = -50;
          minDist = 1725/result.width;
        }
      }
      else{
        objValue = 0;
      }
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

  aData.x = mpu.getAccX();
  aData.y = mpu.getAccY();
  aData.z = mpu.getAccZ();
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

  // Calculates the angle for steering in every 0.1 seconds
  if ((millis() - previousTime) >= 100){
    float dt = (millis() - previousTime)/1000;
    angle = pid.compute((distance_2 - distance_1) + objValue, dt);     // objValue will either be 30 or -30 based on the color of the obstacle
    spos = map(angle, MIN_OUTPUT, MAX_OUTPUT, SERVO_MIN, SERVO_MAX);
    previousTime = millis();
  }

  spos = constrain(spos, SERVO_MIN, SERVO_MAX);
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
