#include <LiquidCrystal.h>


/*#########################CONSTANTS####################*/
#define TARGET_TEMP 52

//How many degrees to tolerate before relaying
#define TEMP_TOLERANCE 1

//how often to read, relay, in milliseconds
#define READ_DELAY (10*1000)
#define RELAY_COOLDOWN (20*60*1000)

//where to write the current and goal temperatures on the LCD
#define LCD_CUR_OFFSET 9, 0
#define LCD_SET_OFFSET 5, 1
#define RELAY_PIN 7
#define THERM_PIN A0
/*######################################################*/


/************************** GLOBALS *********************/
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//the time last read
int lastRead = -(READ_DELAY + 1);
int lastRelay = 0;
int relayStatus = LOW;

/*******************************************************/


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

void setup(){
  lcd.begin(20, 2);
  pinMode(THERM_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop(){
  int now = millis();
  //only read every now and then
  if(now - lastRead > READ_DELAY){
    lastRead = now;
    
    int temp = conv_temp(analogRead(THERM_PIN));
    writeLCD(temp, TARGET_TEMP);
    
    //switch relay if we are over temp and have cooldown
    if(now - lastRelay >= RELAY_COOLDOWN){
      lastRelay = now;
      if(temp > TARGET_TEMP + TEMP_TOLERANCE){
        relayStatus = HIGH;
      }else if( temp <= TARGET_TEMP - TEMP_TOLERANCE){
        relayStatus = LOW;
      }
    }
  }
  digitalWrite(RELAY_PIN, relayStatus);
}
