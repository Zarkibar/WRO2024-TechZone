# WRO2024-TechZone
This is the official repository of the team "Tech Zone" for World Robot Olympiad 2024 in the Future Engineer category. At this repository you can see all of our Codes, Components that were used, Team Photo, Robot's Photos etc.
We are using ESP-8266 for the microcontroller and Husky Lens as the camera for color detection. There are two Sonar Sensors for calculating distance and keep the car in the center of the track. There are two files named `round_one.ino` and `round_two.ino` for Round One and Round two.

# First Round
- Sonar Sensors are used for calculating it's distance from the walls and keeping itself in the center.
- Gyroscope is used to count laps. Whenever the car rotates around 1080 degrees (3 laps), the car will stop.

# Second Round
- Same as first round.
- If the husky lens detects green or red color it will get its width and calculate an estimated distance.
- Then the car will start to slowly steer on the sides as it approaches closer to the obstacle.
