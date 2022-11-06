# Teensy-DShot
Short and simple DShot implementation for Teensy using HardwareSerial.

Currently the following bitrates are supported:
* DShot 150
* DShot 300
* DShot 600

DShot1200 could also be supported, however it would require the use of an alternative clock for the UART. As explained [here](https://forum.pjrc.com/threads/67150-Teensy4-1-MAX-baud-rate?s=755c00bdeb5f97523f030ee7cdeff92d&p=278716&viewfull=1#post278716), this would require changes to hardwareSerial.cpp, which is why it is not being implemented.

## How to use
Select one or more Hardware Serial Ports from the [table](https://www.pjrc.com/teensy/td_uart.html). Connect the Teensy's TX pin to the ESC's signal input. For example, the Teensy4.1's TX Pin for Serial3 is 14.

Your code might look as follows: 
```
#include <Arduino.h>
#include <DShot.h>

DShot motor(&Serial3, DShotType::DShot600);

void setup() { }

void loop() {
    motor.sendThrottle(0, false);
    delayMicroseconds(500);
}
```  
this snippet will initilize your ESC. If the motors are going, make sure to call the "sendThrottle()" method for each ESC at least at 1000 Hz to keep the motors from stopping.
A timer interrupt that calls "sendThrottle()" with the desired throttle could be set up so that calling the function manually is not necessary.

## How it works
### DShot
DShot is a digital protocol for sending commands to ESCs. These commands are transmitted using 16-bit frames, with every frame representing a single command. The frame is divided into three parts:
| Command Bits | Telemetry Bit | CRC Bits |
|:------------:|:-------------:|:--------:|
|      11      |       1       |     4    |

#### Command
There are 2^11 (2.048) possible command values. The first 48 values are special commands while the remaining 2000 values are throttle values from 0% to 100%.
#### Telemetry
The telemetry bit instructs the ESC to send telemetry data using the ESC's telemetry output.
#### CRC
A checksum of both the command and telemetry bits. 
```
crc = (frame ^ (frame >> 4) ^ (frame >> 8)) & 0x0F
```
### UART
UART is hardware that transmits data in serial fashion. The protocol being used sends data in frames, each of which contains four parts:
| Start Bit  | Data Frame  | Parity Bits  | Stop Bits  |
|:----------:|:-----------:|:------------:|:----------:|
|     1      |     5-9     |     0-1      |    1-2     |

As the idle state of the line is high, the start bit is low and the stop bit is high. Binary zero and one are represented by low and high signals, respectively. 

Fortunately, the hardware supports signal inversion, making it possible to mimic DShot using UART. 

### Implementation

In this implementation, the DShot-Frames are sent using UART. Each of the 16 bits in a DShot-Frame is transmitted using a UART-Frame, hence each DShot-Frame is made up of 16 UART-Frames. The TX-Pin is inverted because the signal is normally high when the device is idling, the start-bit is low, and the stop-bit is high. 

Each UART-Frame would perfectly fit a DShot bit if the HardwareSerial supported serial communication in the 6N1 format (a Zero is 37,5% turned on and a One is 75% turned on). However, stretching the 8 bits to suit the frame length will result in inaccuracies because the clock cannot be precisely set to the needed values. 
|     Bits    |   S   |   D1  |   D2  |   D3  |   D4  |   D5  |   D6  |   E   |
|:-----------:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| **Percent** | 0,125 | 0,250 | 0,375 | 0,500 | 0,625 | 0,750 | 0,875 | 1,000 |
|   **Zero**  |   1   |   1   |   1   |   0   |   0   |   0   |   0   |   0   |
|   **One**   |   1   |   1   |   1   |   1   |   1   |   1   |   0   |   0   |

Instead, a UART-Frame with 8N1 is utilized because it is (obviously) supported and the length of 10 bits does not require an odd clock speed. The trade-off is a 6.7% error in length for a zero and a one, which should not be an issue. 

|     Bits    |   S  |  D1  |  D2  |  D3  |  D4  |  D5  |  D6  |  D7  |  D8  |   E  |
|:-----------:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|:----:|
| **Percent** | 0,10 | 0,20 | 0,30 | 0,40 | 0,50 | 0,60 | 0,70 | 0,80 | 0,90 | 1,00 |
|   **Zero**  |   1  |   1  |   1  |   1  |   0  |   0  |   0  |   0  |  0   |   0  |
|   **One**   |   1  |   1  |   1  |   1  |   1  |   1  |   1  |   1  |  0   |   0  |

Theoretically, a UART-Frame with 8N2 is also possible and would lower the error to 3%; however, the frame size of 11 bits would require an odd clock speed, negating the error reduction.

## Ressources
### DShot
* https://brushlesswhoop.com/dshot-and-bidirectional-dshot/
### UART
* https://www.pjrc.com/teensy/td_uart.html
* https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter
