# CellularDodger

This project is an embedded system game that plays on an LCD screen. It was developed for my CS122A class at UC Riverside. The goal of the game is to control a player with a joystick and avoid obstacles. The longer that you last against the obstacles the faster the character moves. The project connects via bluetooth to a server (my computer in the video) that stores a list of phone numbers and the highest all time score. When the highest all time score is beat, the embedded system sends a message to the server to update the highest score. The server updates the score and sends a text message to the list of phone numbers with the new all time high. 

1. Atmega 1284
2. Bluetooth
3. Twilio
4. LCD
5. Joysticks

Click below for video.

[![Demonstration](https://github.com/danialesaid/CellularDodger/blob/master/the_pic.PNG)](https://youtu.be/Ek7ez1i50qI)


This is an example of the message sent by the server.

![png](https://github.com/danialesaid/CellularDodger/blob/master/IMG-20152.png)
