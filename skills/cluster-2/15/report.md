#  Long Range IR Range Sensor

Author: Noah Spahn

Date: 2021 03 07
-----

## Summary
This skill is another distance skill that utilizes the adc. Rather than utilizing sonar as the distance measurement the sensor uses a IR projector detector to detect range. The mechanism detects the distance in a similar means by translating the reflection echo into a return voltage and that is read back into the adc. Through calibration of the IR sensor the distance can be read to the console every 2 seconds. 

## Sketches and Photos


## Modules, Tools, Source Used Including Attribution
The modules involved in this project were a module to read in the IR sensor and to report them to the console. Proceduarlly the same process was repeated to initialise the adc. The IR sensor was calabrated using the spec sheet and a if else tree was used translating the distance adc inputs into readings. The IR sensor in some ways seemed better equipt to handle the task compared to bugs that I ran into with the sonar distancing means.

## Supporting Artifacts


-----
