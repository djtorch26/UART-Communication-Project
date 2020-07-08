# Milestone 1: Communicating with Will Byers

The objective of this milestone was to learn how to utilize the UART communication system that is incorporated inside of the MSP430G2553 Microcrontroller. In doing so, we had to interface with an external RGB led, that would be activated based on an input UART transmission signal. The signal that is sent dictates the amount of bytes that is sent, the PWM signals for each colour of the RGB led, and a stop byte. The microcontroller will in turn modify that signal based on what it should output to another board. For example if sent UART bytes, 0x08(length byte), 0xFF 0x00 0x00(PWM data bytes for RGB led), 0x00 0x00 0xFF(other RGB Data for another led), 0x0D(Stop byte). The software running on the microcontroller will output a signal 0x05 0x00 0x00 0xFF 0x0D. Meaning that the output is the leftover data (-3 bytes) that will be sent to another board to light up a certain colour. 

#Processors Utilized
*MSP430G2553