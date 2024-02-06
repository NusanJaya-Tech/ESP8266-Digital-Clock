#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
int buzzerPin = D5;
int wifiOffLedPin = D7;
int wifiOnLedPin = D6;

unsigned long lastBuzzerTime = 0;
int lastBuzzerHour = -1;

char* ssid = "youssid";
char* password = "youpassword";
const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "id.pool.ntp.org", utcOffsetInSeconds);

char Time[] = "Jam: 00:00:00";
char Date[] = "Tgl: 00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(buzzerPin, OUTPUT);
  pinMode(wifiOffLedPin, OUTPUT);
  pinMode(wifiOnLedPin, OUTPUT);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Getting Started");
  lcd.setCursor(0, 1);
  lcd.print("NTP Clock");
  delay(7000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Done");
  lcd.setCursor(0, 1);
  delay(3000);
  lcd.print("Opening Clock");
  delay(5000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Time);
  lcd.setCursor(0, 1);
  lcd.print(Date);

  WiFi.begin(ssid, password);
  Serial.print("Connecting.");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wifiOffLedPin, HIGH);
    digitalWrite(wifiOnLedPin, LOW);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(wifiOffLedPin, LOW);
  digitalWrite(wifiOnLedPin, HIGH);
  Serial.println("connected");
  timeClient.begin();
}

void loop() {
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();

  hour_ = hour(unix_epoch);

  if (hour_ != lastBuzzerHour) {
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);

      lastBuzzerTime = millis();
      lastBuzzerHour = hour_;
  }

  second_ = second(unix_epoch);
  if (last_second != second_) {
    minute_ = minute(unix_epoch);
    hour_ = hour(unix_epoch);
    day_ = day(unix_epoch);
    month_ = month(unix_epoch);
    year_ = year(unix_epoch);

    Time[12] = second_ % 10 + 48;
    Time[11] = second_ / 10 + 48;
    Time[9] = minute_ % 10 + 48;
    Time[8] = minute_ / 10 + 48;
    Time[6] = hour_ % 10 + 48;
    Time[5] = hour_ / 10 + 48;

    Date[5] = day_ / 10 + 48;
    Date[6] = day_ % 10 + 48;
    Date[8] = month_ / 10 + 48;
    Date[9] = month_ % 10 + 48;
    Date[13] = (year_ / 10) % 10 + 48;
    Date[14] = year_ % 10 % 10 + 48;

    Serial.println(Time);
    Serial.println(Date);

    lcd.setCursor(0, 0);
    lcd.print(Time);
    lcd.setCursor(0, 1);
    lcd.print(Date);
    last_second = second_;
  }
  delay(25);
}
