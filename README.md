# psa-pioneer-remote
PSA to Pioneer car remote converter with Arduino 

Since i upgraded the lousy car radio in my Citroën C3 with a nice bluetooth compatible Pioneer unit, the wheel remote doesn't work anymore. This project aims to fix that.

The wheel remote sends a message on the CAN bus each time a button is pressed/release [1]. 
The Pioneer remote plug expects different resistance values for each action (play, pause, forward...) [2].

The electronics side consists of :
 - an Arduino UNO for prototyping
 - a MCP2515 CAN controller and MCP2551 CAN transceiver to receive CAN messages from the remote
 - a MCP41100 digital potentiometer to talk to the Pioneer car radio
 
The Arduino sketch reads the button state from the CAN bus and sets the digital potentiometer to the proper value.
This project should be adaptable to other car radio using a resistance-based remote input.

TODO :
 - [ ] make the MCP2515 behave to share the SPI bus with the MCP41100
 - [ ] use an ATMega328p instead of an Arduino
 - [ ] power the circuit from the car battery


References : 
 - [PSA CAN protocol](http://autowp.github.io/)
 - [Pioneer WR interface](http://jvde.net/node/7)
