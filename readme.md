# Nurse Assistant

## What it does

Nurse Assistant helps a nurse monitor their various patients by alerting the nurse when they need specific things. This way, the nurse doesn't need to constantly check up on each patient. 

## How we built it

We used two [Adafruit Feather Huzzah](https://www.adafruit.com/product/2821) boards, communicating with each other over WiFi, to build off of. On the "client" board we have the patient's interface, which through various sensors, lets them communicate with the health care professional. 
For example, the motion sensor: when the hand is waved over it, the speaker on the nurse's end informs them of the patient's hunger. On the other hand, the temperature sensor is a more urgent call, transmitting the temperature of the patient's finger when touched, and alerting the nurse when this reaches a critical level.
Finally, the "client" board continually uploads to an IBM Watson IoT interface in order to monitor the entire system over a longer period of time. 

## Challenges we ran into

Communication between different platforms was a major roadblock at times. Especially in uploading information to the Watson interface, which involved protocols (MQTT) and data formats (JSON) that neither of us was particularly experienced or knowledgeable about. In addition, if we had to do this again, we would probably use a different board, or at least an external power supply, considering that the board only outputs 3.3V, and some components required ~5V.

## Things we would add if we had more time

A more complete interface for the patient to use. We would add more specific ways to interact, including buttons, and perhaps even voice control. We would have also added an LCD screen to display information (perhaps from multiple patients) on the nurse's end.

## What we learned

The biggest thing is connecting boards to each other and the Internet through WiFi, and it is something we will definitely make use of in the future. 