# nano-rti

### Reference for signal encoding/decoding

[Data Formats for IR Remote Control](https://www.vishay.com/docs/80071/dataform.pdf)

[Signal Encoding Techniques - CSE 3461/5461 Ohio State](http://web.cse.ohio-state.edu/~athreya.14/cse3461-5461/Cse3461.C.SignalEncoding.09-04-2012.pdf)

The RTI Signal seems to be using [Manchester Encoding](https://en.wikipedia.org/wiki/Manchester_code) as referenced in above document(s).

### Reference for State Machines

[UML State Diagram](http://agilemodeling.com/style/stateChartDiagram.htm)

### Reference for Arduino Nano V3

[Gravitech Doc](https://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf)

[Nano Ethernet Shield](https://robotdyn.com/nano-v3-ethernet-shield-enc28j60.html)

[UIPEthernet Library](https://github.com/UIPEthernet/UIPEthernet); installed through PlatformIO `lib_deps = UIPEthernet@^2.0.6`

### Reference for Arduino ISRs

Pins 2 and 3 are usable for interrupts on the Nano

[attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)
