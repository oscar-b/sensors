#include "ocLib.h"

PulseMeter::PulseMeter(uint8_t num) {
	pin = num;
	firstPulse = true;

	pinMode(pin, INPUT);
}

void PulseMeter::check() {
	uint32_t delta, currentMicros = micros();
	uint16_t tempPower, _median;

	//Shift state history to the left, and read new value of the pin
	state <<= 1;
	state |= bitRead(PIND, pin);

	//Pulse rising edge (7 times low followed by 1 time high signal)
	if (state  == B00000001) {
		// Always count pulses
		pulses++;

		//If this is the first pulse, just note the time and return, this avoids
		//false high initial power calculations.
		if (firstPulse) {
			firstPulse = false;
			lastPulse = currentMicros;
			Serial.print(pin);
			Serial.println(": Ignoring power calculation for first pulse");
			return;
		}

		delta = currentMicros - lastPulse;
		tempPower = calculatePower(delta);

		power.add(tempPower);

		lastPulse = currentMicros;

		checkTimer.set(powerToDeltaMilliSeconds(tempPower - 1));
		
		// Debug printing
		Serial.print(pin);
		Serial.print(": NOW: ");
		Serial.print(tempPower);
		Serial.print("W -- MEDIAN: ");
		if (power.getMedian(_median) == power.OK) {
			Serial.print(_median);
			Serial.print("W");
		}
		Serial.println(" ");
		
		
	} else if (checkTimer.poll()) {
		//Calculate simulated power as if we got a pulse. If this is smaller
		//than power, we know that the new momentary power must be smaller than
		//power, so we can go ahead and lower it right away.
		tempPower = calculatePower(currentMicros - lastPulse);
		if (power.getMedian(_median) == power.OK) {
			if (tempPower < _median) {
				//averagePower(tempPower);
				power.add(tempPower);
			}
		}

		// Debug printing
		Serial.print(pin);
		Serial.print(": ... SIMULATED NOW: ");
		Serial.print(tempPower);
		Serial.print("W -- MEDIAN: ");
		if (power.getMedian(_median) == power.OK) {
			Serial.print(_median);
			Serial.print("W");
		}
		Serial.println(" ");

		// Check again in 2s
		checkTimer.set(2000);

	}
}

// Calculate power in W, based on 1000 imp/kWh
uint16_t PulseMeter::calculatePower(uint32_t delta) {
	return uint16_t( (3600000000.0 / delta) + 0.5);
}

// Calculate the number of seconds until the next pulse, for the specified power
uint16_t PulseMeter::powerToDeltaMilliSeconds(uint16_t power) {
	if (power <= 60)
		return (60000);
	else
		return uint16_t(3600000UL / power);
}

// Calculate average power based on 4 last readings (3/4 of old, 1/4 of new)
//void PulseMeter::averagePower(uint16_t newPower) {
//	power = uint16_t((power * 3UL) + (newPower) + 0.5) >> 2;
//}
