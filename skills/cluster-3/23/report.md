# Triple-axis Accelerometer

Author: Noah Spahn
Date: 2021-03-19
-----

## Summary
This skill utilized the accelerometer and the i2c to create an accelerometer read. Issues were reading the register, but after trying a modification by the group, we realized there was a configuration issue as I could see a lack of ability to read from my registers when run on my hardware. I was able to debug this and eventually read pitch and other values to the console. The second video represents the skill as the first didn't have the rise working correctly on the console. 

## Sketches and Photos
https://drive.google.com/file/d/1-zI8EMISSCI5QHuP6gzTVC8-U5ftzufE/view?usp=sharing (pitch not working)
https://drive.google.com/file/d/1-zI8EMISSCI5QHuP6gzTVC8-U5ftzufE/view?usp=sharing (pitch corrected)

## Modules, Tools, Source Used Including Attribution
The modules were adapted from the example give,https://esp32.com/viewtopic.php?t=11760 and https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing. as well as https://www.adafruit.com/product/4097

## Supporting Artifacts


-----
