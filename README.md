# DIY Arduino Lasertag

This is an Arduino powered Infrared laser tag set I created. It is powered using an Arduino Nano. The set has 6 primary functions and features such as, ammunition counter, hit-detection from other laser tag guns, IR data analysis, reloading button paired with a slider, LCD user interface and finally a play feedback subsystem as well as features such as 8 different weapons, filtration of teamfire and a health counter.

![PhotoOfGuns](https://i.imgur.com/7ouMkQu.jpeg)

## Description

Explanation


## 3D printed parts
Each laser tag gun consists of 13 3D printed parts. The guns structure uses a middle black frame which holds a majority of the electronic components and microcontroller, while 4 large shells cover both sides of the black frame.

Frame:
![Frame](https://i.imgur.com/RXxQxVe.png)

Frame with shells and accessories:

![Full build](https://i.imgur.com/IBLJHwQ.png)

## Black frame 3D printed parts:
- 

Components:
- 1x IR LED

## 3D printed parts

I designed the parts in Fusion 360 and exported them as a 3mf.
The recommended print settings I would use are:
- 0.1mm layer height
- Black PLA plastic worked best

## Assembly



## Code

The code is very straight forward and simple, but a calibration will need to be done to find the correct threshold.
```
//Definitions to save memory
//IRIN_2 pin
#define IRIN_2 A0

//IRIN_1 Interrupt pins
#define IRIN_1_I1 2
#define IRIN_1_I2 3

//To find out what value you need to set threshold to, set debug mode to true and turn the wheel clockwise and record the value
//    then turn the wheel anti-clockwise and record the value. You should set threshold to halfway between the results 
#define DEBUG_MODE false
#define THRESHOLD 100

//This variable stores the positio
int position = 0;

//Setup
void setup() {
  //Set pinmodes for IRIN
  pinMode(IRIN_1_I1, INPUT);
  pinMode(IRIN_1_I2, INPUT);
  pinMode(IRIN_2, INPUT);

  //Begin Serial Interface
  Serial.begin(9600);

  //Attach interrupts to IRIN_1
  attachInterrupt(digitalPinToInterrupt(IRIN_1_I1), IRIN_1_goingHigh, RISING);
  attachInterrupt(digitalPinToInterrupt(IRIN_1_I2), IRIN_1_goingLow, FALLING);
}

void loop() {
}

//When IRIN_1 goes high
void IRIN_1_goingHigh()
{
  //Get value of IRIN_2
  int irin_2_val = analogRead(IRIN_2);

  //DEBUG MODE
  if (DEBUG_MODE)
  {
    Serial.println(irin_2_val);
    return;
  }

  //Check value against threshold
  if (irin_2_val > THRESHOLD)
  {
    //Going clockwise
    position++;
  }
  else
  {
    //Going anticlockwise
    position--;
  }

  //Return value to serial monitor
  Serial.println(position);
}

//When IRIN_2 goes low
void IRIN_1_goingLow()
{
  //DEBUG MODE
  if (DEBUG_MODE)
  {
    return;
  }

  //Get value of IRIN_2
  int irin_2_val = analogRead(IRIN_2);

  //Check value against threshold
  if (irin_2_val < THRESHOLD)
  {
    //Going clockwise
    position++;
  }
  else
  {
    //Going anticlockwise
    position--;
  }

  //Return value to serial monitor
  Serial.println(position);
}
```

## Version History

* 1.0
    * Initial Release
