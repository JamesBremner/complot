# complot

_Initialization_
- Open com port
- Create polling timer
- Create plot update timer

_On polling timer_
- run async read

_On read complete_
- Frame new data and copy new data into FIFO buffer
- Trim FIFO to desired plot length

_On plot update timer_
- Update plot with data in FIFO buffer

# Usage

- Set up a virtual null modem between COM ports ( use default 8 & 9 )
- Start comdevicesim app
- Connect port
- Start complotter app
- Connect port
- in device simulator set frame rate
- in device simulator click TX button
- Click TX button in
