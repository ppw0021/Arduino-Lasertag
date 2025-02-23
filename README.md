# DIY Arduino Lasertag

This is an Arduino powered Infrared laser tag set I created. It is powered using an Arduino Nano. The set has 6 primary functions and features such as, ammunition counter, hit-detection from other laser tag guns, IR data analysis, reloading button paired with a slider, LCD user interface and finally a play feedback subsystem as well as features such as 8 different weapons, filtration of teamfire and a health counter.

![Guns](https://github.com/user-attachments/assets/88debcd2-ec43-4b69-94da-1d8358f4dd7f)

## Description

Explanation


## 3D printed parts
Each laser tag gun consists of 13 3D printed parts. The guns structure uses a middle black frame which holds a majority of the electronic components and microcontroller, while 4 large shells cover both sides of the black frame.

Frame:

![FrameRender](https://github.com/user-attachments/assets/976a61c3-f0de-472d-a386-11e36a994a1b)

Exploded assembly:

![FullRenderExploded](https://github.com/user-attachments/assets/48e5b606-043e-456d-8fcd-e841506ee68c)

Frame with shells and accessories:

![FullRender](https://github.com/user-attachments/assets/f6366cc2-6ae3-4eae-a8d7-08fa32ee549a)

As this was a long time and I did not know any better, I created the 3D models in SketchUp (🤮) and exported them as very very broken STLs. I used Netfabb to fix the models and make them manifold.
I recommend using Black, White and Clear PLA or ABS plastic at 0.1mm layer height for best results. 

## Electronics

The microcontroller for this project is an Arduino Nano with an ATmega328p chip onboard.
The following components are required for each laser gun:
- 1x IR LED  (For signal transmission)
- 1x 31MM biconvex lens 27MM focal point  (For straightening the IR light from the IR LED)
- 6x IR Receiver TSOP4838 DIP3  (For receiving transmissions from other laser guns)
- 1x i2c 16x2 LCD display modules  (For displaying UI to user)
- 1x YX5300 UART Serial MP3 player  (For playing sounds)
- 1x MicroSD card  (For storing sounds)
- 3x DC3v Button Vibrating Motors  (For providing haptic feedback)
- 6x push buttons  (For navigating UI and reload switches)
- 1x PAM8403 digital amplifier (For amplifying the audio signal from the MP3 Player)
  - 1x 4omh speaker
  - 1x 4.7uF capacitor
  - (Please note these parts are quite arbitrary and were subject is guesswork, results may vary)
- 7x 1k ohm resistors (Various pulldown)
- 1x NPN transistor
- 

## Code

```
CODE HERE
```

## Version History

* 1.0
    * Initial Release
