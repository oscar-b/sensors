sensors
=======

PulsesEnergyMeter
-----------------

Sensor for collecting pulses from the S0 output of three energy meters.
Calculates momentary power use, which is mean filtered when sent by
RFM12B.

The code also predicts when the next pulse should come, and if it hasn't
(declining power use) it will start lowering the power right away
instead of waiting for the next pulse to arrive.
