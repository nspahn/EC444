#  PWD LED Control

Author: Noah Spahn
Date: 2021-03-20
-----

## Summary
In this skill the pwd was usitlised in the esp 32 by staggering the waves given a users input for a given light brightness. The funcitons were initially borrowed from the example ledc and the timing was reduced from 3 seconds per step to a single step every 250ms. This was achived by reducing the initial 3000 LEDC_TEST_FADE_TIME. The brightness was achived by stagnating the pwd to reduce the intensity of the led. 
## Sketches and Photos
Video in drive of fade - 
https://drive.google.com/file/d/14gBBNzVeKfr6pR-G1ZbnBdWWElm-WB-F/view?usp=sharing
https://drive.google.com/file/d/1-zI8EMISSCI5QHuP6gzTVC8-U5ftzufE/view?usp=sharing

## Modules, Tools, Source Used Including Attribution
Main modules included the initial proticol borrowed from the example ledc which was modified to increase the time between delay steps and to recive user input. The system asks for user input then loops to reach that number of steps in light adjustment. This system was designed with units 0-9 in mind for valid input. at which point the ledc channel set fade with time is lowered to step every 250ms delay and the LEDC_Test_Duty is adjusted to fit intensity needs.

## Supporting Artifacts


-----
