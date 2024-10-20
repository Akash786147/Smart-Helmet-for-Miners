#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <LiquidCrystal.h>

RH_ASK driver;

const int rs = 12, 
          en = 10,  // Changed to pin 10
          d4 = 5, 
          d5 = 4, 
          d6 = 3, 
          d7  = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  analogWrite(6, 60); // Set contrast
  
  if (!driver.init())
    Serial.println("init failed");
}

void loop() {
  uint8_t buf[100];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';
    Serial.print("Received: ");
    Serial.println((char*)buf);

    displayMessage((char*)buf);
  }
}

void displayMessage(char* message) {
  lcd.clear();

  for (int i = 0; i < strlen(message); i++) {
    lcd.print(message[i]);
    delay(200);
  }

  lcd.setCursor(16, 1);
  lcd.autoscroll();
  
  for (int i = 0; i < strlen(message); i++) {
    lcd.print(message[i]);
    delay(400);
  }
  
  lcd.noAutoscroll();
  delay(1000); // Optional delay before clearing the LCD
  lcd.clear();
}
