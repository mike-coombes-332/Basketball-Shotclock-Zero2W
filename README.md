# Basketball-Shotclock-Zero2W
A basketball shot clock developed in 5 days, to allow a local league to play in school gyms. 

Code running on an RP ZERO2W SBC with a RP PICO used as a PWM source for audio. (Zero2W GPIO6 used an a EnableBuzzer signal to Pico)

HDMI splitter used for Video out to 2 PC monitors (HDMI sent 50M across Ethernet)

PC Numberpad keyboard used to control the app
"1" = Reset Count to 14S (FIBA)
"2" = Reset Count to 24S (FIBA)
"3" = Toggle between Running and Paused
"*" = Sound Buzzer

================================
SFML Graphics library's in use
================================

Other notes... The RP Zero2W is a great SBC, but it is quite RAM limited, so rather than using BT to send audio. I chose to use a RP RICO and that way the delay between the Display and Buzzer was deterministic and could be acounted for.

Note as descibed this was a rushed project, and certainly could be improved significantly. My hope one day it to make a very low cost shotclock.


