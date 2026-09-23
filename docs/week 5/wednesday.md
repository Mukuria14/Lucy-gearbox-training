# Serial and parallel communication
Serial communication transfers data one bit at a time over a single data line. It is slower than parallel in theory but more reliable over long distances and uses fewer wires.

Parallel communication transfers multiple bits simultaneously using multiple data lines. It is faster for short distances but prone to noise and expensive due to many wires.

# communication protocols
Communication protocols define rules and standards that devices use to exchange data. They specify data format, timing, clocking, error handling, and how devices synchronize communication.

# synchronous and asynchronous communication.
## synchronous communication.
- Uses a shared clock between sender and receiver.
- Data is transmitted with precise timing.
- Faster and reliable for continuous data flow.

## asynchronous communication.
- No shared clock; communication depends on start and stop bits.
- Simpler, cheaper, but slightly slower.
- Used in UART-based communication.

# serial peripheral interface. (SPI)
 SPI is a common communication protocol used by many different devices. For example, SD card reader modules, RFID card reader modules, and 2.4 GHz wireless transmitter/receivers all use SPI to communicate with microcontrollers.

# key lines
MOSI - Master out slave in.Line for the master to send data to the slave.
MISO - Master in slave out.Line for the slave to send data to the master.
SCLK - Clock signal from master. 
SS/CS - Slave Select / Chip Select (Master chooses which slave to talk to)
