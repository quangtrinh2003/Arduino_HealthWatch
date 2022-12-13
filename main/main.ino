#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <LiquidCrystal_I2C.h>
#define TCAADDR 0x70
#define button D5

//Init AXDL345 && LCD1602
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(0x53);
float threshold = 150;
int steps = 0;
float distance = 0;
int choice = 1;

//Init webserver && NTP 
WiFiUDP ntpUDP;
const int utcOffsetInSeconds = 7*60*60;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
WiFiServer server(80);
const char *ssid     = "Thuan";
const char *password = "0908270035";
String weekDays[7]={"CN.", "Th2", "Th3", "Th4", "Th5", "Th6", "Th7"};
String date;

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

void setup() 
{
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  server.begin();
  timeClient.begin();

  Serial.println("Accelerometer Test"); Serial.println("");
  /* Initialise the sensor with the I2C address */
  if(!accel.begin(0x53))
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  
  //start lcd
  lcd.init();
  lcd.backlight();
  khoiDongLCD();
  lcd.createChar(0, shoes);
  lcd.createChar(1, distanceChar);
  lcd.clear();
  // set input button
  pinMode(button, INPUT);
}

void mainScreen()
{
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
}

void subScreen()
{
  timeClient.update();
  lcd.setCursor(4,0);
  date = weekDays[timeClient.getDay()] + "," + timeClient.getHours() + ":" + timeClient.getMinutes();
  if(timeClient.getHours() >= 12)
  {
    date += "PM";
  }
  else
  {
    date += "AM";
  }
  lcd.print(date);
  lcd.setCursor(1, 1);
  lcd.print(WiFi.localIP());
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
  
  if(digitalRead(button) == HIGH)
  {
    choice += 1;
  }

  if(choice % 2 == 0)
  {
    lcd.clear();
    subScreen();
  }
  else
  {
    lcd.clear();
    mainScreen();
  }
  delay(1000);
}