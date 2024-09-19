#include <Servo.h>
#include <Wire.h>
#include <MPU6050_light.h>


#define m1 D8
#define m2 D7
#define trigPin1 9
#define echoPin1 10
#define trigPin2 D4
#define echoPin2 D3

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

int spos = 100;  // max = 140, min = 60
long distance_1 = 0;
long distance_2 = 0;
bool stopped = false;

void setup(){
  Serial.begin(115200);
  Wire.begin();

  if (mpu.begin() != 0) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }

  mpu.calcOffsets();

  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  servo.attach(D5);
}

void loop()
{
  measureRotation();
  //measureDistance();

  servo.write(60);

  if (gData.z > -1070 && gData.z < 1070 && !stopped) forward();
  else stop();
}

void measureRotation()
{
  mpu.update();
  gData.x = mpu.getAngleX();
  gData.y = mpu.getAngleY();
  gData.z = mpu.getAngleZ();

  Serial.print("X-Axis: ");
  Serial.print(gData.x);
  Serial.print("   Y-Axis: ");
  Serial.print(gData.y);
  Serial.print("   Z-Axis: ");
  Serial.println(gData.z);
}

void measureDistance()
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
  // Serial.print("Distance: ");
  // Serial.print(distance_2);
  // Serial.println(" cm");
  
  if(distance_1 > distance_2){
    spos = map(distance_2, 0, 60, 60, 100);
    spos = constrain(spos, 60, 100);
  }
  if(distance_1 < distance_2){
    spos = map(distance_1, 0, 60, 140, 100);
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
