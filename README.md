# Introduction to Robotics (2021 - 2022)
Hi! This is the repo where I'll post my homeworks for the Introduction to Robotics course, taken optionally in the 3rd year at the Faculty of Mathematics and Informatics at the University of Bucharest. Below you'll find pictures and detailed descriptions of the implementations for each homework. Have at least as much fun browsing these as I had making them.

Also you're free to distribute or use this code however you like, as long as you keep it free (free as in freedom) and open source (see attached GNU GPL License v3.0 for more details).
  
Index:
* [Homework 1 - RGB LED controlled by 3 potentiometers](#rgb_led)
* [Homework 2 - Traffic Lights simulation in 4 stages, triggered by button](#traffic_lights)
* [Homework 3 - EMF Detector, catches all kinds of ghosts and demons ](#emf_detector)

...
  
## RGB_LED
### Homework 1
Our first homework consisted of making use of 3 potentiometers to control each colour of an RGB LED. I read the values of the potentiometers and digitally map them to current intensity values to output to each LED colour. Pretty simple really. Here's some pictures:

<img src="https://raw.githubusercontent.com/Costinteo/IntroductionToRobotics/main/homework1/side_view_1.jpg" width="400" />  <img src="https://raw.githubusercontent.com/Costinteo/IntroductionToRobotics/main/homework1/side_view_2.jpg" width="400" /><img src="https://github.com/Costinteo/IntroductionToRobotics/blob/main/homework1/top_down.jpg?raw=true" width="400" />

And here's the video:

https://www.youtube.com/watch?v=pI68Qa9GIf4E

## TRAFFIC_LIGHTS
### Homework 2
Second assignment was to simulate some traffic lights. Basically, we have a pair of lights for pedestrians that presumably want to cross the street (green and red) and 3 lights for the cars (green, yellow and red). The simulation starts when the button is pressed. So after counting down 10 seconds, the simulation proceeds to stage 2 (yellow for cars). Then, for stage 3, we have red for cars and green for pedestrians and the buzzer starts buzzing. Final stage is green flashing for pedestrians and buzzer syncing with the flashes. And finally, the stages loop back to beginning. Here's some pictures:

<img src="https://raw.githubusercontent.com/Costinteo/IntroductionToRobotics/main/homework2/front_view_1.jpg" width="400" />  <img src="https://raw.githubusercontent.com/Costinteo/IntroductionToRobotics/main/homework2/front_view_2.jpg" width="400" /><img src="https://raw.githubusercontent.com/Costinteo/IntroductionToRobotics/main/homework2/side_view.jpg" width="400" /> <img src="https://raw.githubusercontent.com/Costinteo/IntroductionToRobotics/main/homework2/top_view.jpg" width="400" />

And here's the video:

https://www.youtube.com/watch?v=18-YGTBy0yA

## EMF_DETECTOR
### Homework 3
[DISCLAIMER] I really took the time off for this homework, nothing too creative here, folks :(

Third assignment was to make a simple EMF detector. Gotta fight the undead somehow. Best practice is detection and prevention first. So I made a really simple EMF detector. It detects EMF through a paperclip antenna and sends it to the Arduino. The code then decides how many leds to light up and how fast the buzzer buzzes. Here's some pictures:

<img src="https://github.com/Costinteo/IntroductionToRobotics/blob/main/homework3/front_view.jpg?raw=true" width="400" />  <img src="https://github.com/Costinteo/IntroductionToRobotics/blob/main/homework3/top_view.jpg?raw=true" width="400" /><img src="https://github.com/Costinteo/IntroductionToRobotics/blob/main/homework3/side_view_1.jpg?raw=true" width="400" /> <img src="https://github.com/Costinteo/IntroductionToRobotics/blob/main/homework3/side_view_2.jpg?raw=true" width="400" />

And here's the video:

https://www.youtube.com/watch?v=zecxh1xtOQE


