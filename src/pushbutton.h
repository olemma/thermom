#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#define DEBOUNCE_DELAY 50
/**
* PushButton represents a push button with debounce.
* Instantiate the constructor inside the setup() code
* to set the pin mode correctly.
*/
class PushButton{
public:
	unsigned long lastDebounce;
	int state, lastState, pin;

	PushButton(): pin(pin), state(LOW), lastState(LOW), lastDebounce(0UL)
	{
		;
	}

	PushButton(int pin)
	: pin(pin), state(LOW), lastState(LOW), lastDebounce(0UL)
	{
		pinMode(pin, INPUT);
	};

	/**
	* Check if button is pressed. This code probably needs to
	* be run constantly under loop()
	*
	* @return true if button is pressed, false otherwise
	*/
	bool isPressed()
	{
		int reading = digitalRead(this->pin);
		bool retVal = false;

		if(reading != lastState){
			this->lastDebounce = millis();
		}
		
		if(millis() - this->lastDebounce > DEBOUNCE_DELAY){
			if(reading != this->state){
				this->state = reading;
				retVal = (this->state == HIGH);
			}
		}
		this->lastState = reading;
		return retVal;
	}
};

#endif
