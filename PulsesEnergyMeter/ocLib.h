#include <Arduino.h>
#include <JeeLib.h>
#include "RunningMedian.h"

class PulseMeter {
	private:
		uint8_t pin, state, firstPulse;
		uint16_t currentPower;
		uint32_t lastPulse;
		MilliTimer checkTimer;

		uint16_t calculatePower(uint32_t delta);
		uint16_t powerToDeltaMilliSeconds(uint16_t power);
		void averagePower(uint16_t newPower);

	public:
		uint16_t pulses;
		RunningMedian<uint16_t, 5> power;

		//Constructor
		inline PulseMeter (uint8_t num) : pin (num), firstPulse (true) {}

		void check();

};
