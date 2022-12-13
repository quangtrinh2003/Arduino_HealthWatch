#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <LiquidCrystal_I2C.h>
#define TCAADDR 0x70

LiquidCrystal_I2C lcd(0x27, 16, 2);
/* Assign a I2C to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(0x53);
float threshold = 120;
int steps = 0;
float distance = 0;

byte shoes[] = {
  B00000,
  B00000,
  B01000,
  B01000,
  B01100,
  B01010,
  B01110,
  B00000
 };

byte distanceChar[] = {
  B11111,
  B11011,
  B11101,
  B11011,
  B10111,
  B11011,
  B10111,
  B00000
 };

 byte loadingNode[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
 };

void khoiDongLCD()
{
  lcd.createChar(0, loadingNode);
  lcd.setCursor(4, 0);
  lcd.printstr("Starting");
  for(int i = 0; i < 16; i++)
  {
    lcd.setCursor(i, 1);
    lcd.write(0);
    delay(600);
  }
  lcd.clear();
}

void tcaselect(uint8_t i) 
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("Accelerometer Test"); Serial.println("");
  /* Initialise the sensor with the I2C address */
  if(!accel.begin(0x53))
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);

  tcaselect(2);
  lcd.init();
  lcd.backlight();
  khoiDongLCD();
  lcd.createChar(0, shoes);
  lcd.createChar(1, distanceChar);
}

void loop() 
{
  sensors_event_t event; 
  accel.getEvent(&event);
  float vector = sqrt((event.acceleration.x*event.acceleration.x) * (event.acceleration.y*event.acceleration.y) * (event.acceleration.z*event.acceleration.z));
  if(vector > threshold)
  {
    steps = steps + 1;
    distance = distance + 0.3;
  }
  tcaselect(2);

  lcd.setCursor(0,0);
  lcd.write(0);
  lcd.setCursor(1,0);
  lcd.printstr(":");
  lcd.setCursor(2, 0);
  lcd.print(steps);
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(1, 1);
  lcd.printstr(":");
  lcd.setCursor(2, 1);
  lcd.print(distance);
  delay(1000);
}
