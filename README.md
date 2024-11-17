This project aim to create a circuit board, which can help you control an ouptut via a relay.

It likes a normal relay board in the market. But this can be controlled via USB with HID interface.

How to control:

+ Via Python script
+ Via a Simple HID program

+ The relay is controlled via ENDPOINT1
  + With the first byte of data, ENDPOINT1[0] = 0x02 => Relay will be closed
  + With the first byte of data, ENDPOINT1[0] = 0x01 => Relay will be openned again
