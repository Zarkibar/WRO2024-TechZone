# WRO2024-TechZone
This is the official repository of the team "Tech Zone" for World Robot Olympiad 2024 in the Future Engineer category. At this repository you can see all of our Codes, Components that were used, Team Photo, Robot's Photos etc.
We are using ESP-8266 for the microcontroller and Husky Lens as the camera for color detection. There are two Sonar Sensors for calculating distance and keep the car in the center of the track. There is only one file for the ESP-8266 named `main.ino` that contains the code for both Round One and Two.

# First Round
- Sonar Sensors are used for calculating it's distance from the walls and keeping itself in the center.
- Gyroscope is used to count laps. Whenever the car rotates around 1080 degrees (3 laps), the car will stop.

# Second Round
