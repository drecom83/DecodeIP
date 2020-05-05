# DecodeIP
Determine the given IP number of a device using 4 LEDs and a button

An IP has 4 block of each max 3 numbers

There are 4 LEDs. From left to right:
1. Blue: Acknowledge LED
2. Yellow 1
3. Yellow 2
4. Yellow 3

## Usage
Pushing the button for 1 second lights up the first LED. Each extra second lights up the next LED.<br>
Releasing the button determines the IP-block to determine.
A block has always 3 numbers (e.g. 001, 192, 010)

1 sec: XXX.xxx.xxx.xxx<br>
2 sec: xxx.XXX.xxx.xxx<br>
3 sec: xxx.xxx.XXX.xxx<br>
4 sec: xxx.xxx.xxx.XXX<br>

6 sec: sets 'renew the IP address'

When a block is selected, the next step is to determine the number.<br>
This is done by pushing the button shortly (less than 1 second).<br>
The yellow LEDs are the counters. When the counter reaches the number, the blue LED will light up.<br>
The counting an light up of the blue LED will continue a couple of times.<br>
When the button is pressed shortly again, the second number will be determined. the same goes for the third number.<br>
Pressing the button shortly after the third number will bring you back to the first number.<br>

An exception is made for the number 0 (zero). A zero is shown as the blue LED and the last yellow LED.

Example:<br>
196.168.4.1   ->  196.168.004.001

- is off<br>
+ is on<br>

|Number |Blue   |Yellow 1|Yellow 2|Yellow 3|     |
|:------|:------|:-------|:-------|:-------|:----|
|*1*96  |+      |+       |-       |-       |1x   |
|1*9*6  |+      |+       |+       |+       |3x   |
|19*6*  |+      |+       |+       |+       |2x   |
|:------|:------|:-------|:-------|:-------|:----|

