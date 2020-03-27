This repo houses the i2c 'non-blocking' multiple connection library 
and example arduino files for using them.It is adapted from Patrick 
Allsion's Tiva TwoWire version of a 'non-blocking' library as well as 
the most recent tiva-core release TwoWire library on their github as of 
03/27/20. 

By 'non-blocking' we mean the calls for reading/writing using the 
TwoWire libraries have been adjusted to fall through a state 
machine, returning status of the bus. In other words, anything similar 
to "while(ROM_I2CMasterBusBusy(MASTER_BASE))" has been replaced with 
"if(ROM_I2CMasterBusBusy(MASTER_BASE))".

This is useful for our purposes of having the microcontroller 
(Tm4C123GXL launchpad) do other tasks instead of waiting for the I2C 
bus to finish. Our uC is communicating with many devices simultaneously 
(8 serial, 2 I2C, >2 ADCs, some GPIOs), and this device is central to 
collecting from other uCs (3 of those serial connections are other 
laucnhpads to more sensors). Therefore, these libraries are necessary to 
minimize the 'blocking' of the uC. 

The first example allows for viewing the state via a Serial poll method. 
Second example is the typical read/write statemachine with two slaves. 
This has not been tested with uC being the slave device. 
