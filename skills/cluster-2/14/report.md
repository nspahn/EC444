#  Ultrasonic Range Sensor

Author: Noah Spahn

Date: 2021-03-06 
-----

## Summary
In this skill the range finder accessory was used to report range data to the console. The sensor had accuracy within about 2 meters and utilized the adc voltage input as a method to calibrate the distance as well as the power inputs to supply power to the wave generator. Using the wave generator to echo distance the result was reported to the console every 2 seconds. A capacitor was used however I was unable to determine if this reduced noise in this case. my results also had the issue of reporting the first result multiple times and not a loop of continous results. The beta value used with this sensor was similar to the documentaiton. As for witring method, I used the BW out on the sensor as I felt it was easiest to read within the consol. 

## Sketches and Photos
included in img foler

## Modules, Tools, Source Used Including Attribution
The modules include a module to read the range and report it back to the console. The module involved calibration given the ultrasonic sensor as well as reporting to the converted results. Procedurally the adc had to be initiated and calibrated given the chosen venue of output for the sensor. 


## Supporting Artifacts



