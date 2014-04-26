#include <LiquidCrystal.h>

#include "pushbutton.h"
#include "pins.h"

/**************** CONSTANTS *****************************/
//How many degrees to tolerate before relaying
const int TEMP_TOLERANCE =  1;

//how often to read, relay, in milliseconds
const unsigned long READ_DELAY = 500UL;
const unsigned long RELAY_COOLDOWN = 20000UL;

//where to write the current and goal temperatures on the LCD
#define LCD_CUR_OFFSET 9, 0
#define LCD_SET_OFFSET 5, 1

/************************** GLOBALS *********************/
LiquidCrystal lcd(
		LCD_RS_PIN,
		LCD_ENABLE_PIN,
		LCD_D4_PIN,
		LCD_D5_PIN,
		LCD_D6_PIN,
		LCD_D7_PIN
);


int targetTemp = 70;
//the time last read
unsigned long lastRead = 0UL;
unsigned long lastRelay = 0UL;
unsigned long relayStatus = LOW;

PushButton up, down;

/************************ CODE  ***********************/

void setup(){
	down = PushButton(DOWN_BUTTON_PIN);
	up = PushButton(UP_BUTTON_PIN);
	lcd.begin(20, 2);
	pinMode(THERM_PIN, INPUT);
	pinMode(RELAY_PIN, OUTPUT);
}


/**
 * Convert a resistance reading to fahrenheit teperature
 */
int conv_temp(int RawADC) {
	double temp = log(((10240000/RawADC) - 10000));
	temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp ))* temp );
	temp = temp - 273.15;            // Convert Kelvin to Celcius
	temp = (temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
	return int(temp);
}

/* Write the current and target temperature to the LCD */
void writeLCD(int curTemp, int setTemp){
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Current:");
	lcd.setCursor(LCD_CUR_OFFSET);
	lcd.print(curTemp);

	lcd.setCursor(0, 1);
	lcd.print("Set:");
	lcd.setCursor(LCD_SET_OFFSET);
	lcd.print(setTemp);
}

void loop(){
	unsigned long now = millis();

	//check buttons to set temperature
	if(up.isPressed()){
		targetTemp++;
	}
	if(down.isPressed()){
		targetTemp--;
	}

	//only read every now and then
	if(now - lastRead >= READ_DELAY){
		lastRead = now;

		int temp = conv_temp(analogRead(THERM_PIN));
		writeLCD(temp, targetTemp);

		//switch relay if we are over temp and have cooldown
		if(now - lastRelay >= RELAY_COOLDOWN){
			if(temp > targetTemp + TEMP_TOLERANCE){
				if(relayStatus == LOW){
					lastRelay = now;
				}
				relayStatus = HIGH;
			}else if( temp < targetTemp - TEMP_TOLERANCE){
				if(relayStatus == HIGH){
					lastRelay = now;
				}
				relayStatus = LOW;
			}
		}
	}

	//set relay!
	digitalWrite(RELAY_PIN, relayStatus);
}
