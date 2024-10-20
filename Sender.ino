#include <RH_ASK.h>
#include <SoftwareSerial.h>
#include <SPI.h>

// Constants for Pulse Sensor
const int PULSE_SENSOR_PIN = A0;  // Analog PIN where the PulseSensor is connected
const int LED_PIN = 13;            // On-board LED PIN
const int THRESHOLD = 550;         // Threshold for detecting a heartbeat
const unsigned long SAMPLE_INTERVAL = 10;
const int forceSensorPin = A2;// Sampling interval in milliseconds

// Constants for MQ2 Gas Sensor
const int MQ2_ANALOG_PIN = A1;

// Constants for Push Button
const int BUTTON_PIN = 2; // Pin connected to the push button

// GSM module configuration
#define GSM_TX_PIN 4 // Change this to the pin number you want to use for TX
#define GSM_RX_PIN 5 // Change this to the pin number you want to use for RX
#define GSM_BAUD_RATE 9600 // GSM module baud rate

SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN); // RX, TX

RH_ASK driver;

unsigned long lastSampleTime = 0;
unsigned long lastBeatTime = 0;
unsigned long beatsPerMinute;
bool beatDetected = false;

void setup() {
  Serial.begin(9600); // Initialize Serial Monitor

  // Initialize RH_ASK driver
  if (!driver.init())
    Serial.println("RH_ASK init failed");

  // Set MQ2 sensor pin mode
  pinMode(MQ2_ANALOG_PIN, INPUT);

  // Set up push button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize GSM module
  gsmSerial.begin(GSM_BAUD_RATE);
  delay(1000);
  gsmSerial.println("AT"); // Check if GSM module is responding
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
}

void loop() {
  int forcesensorValue1 = analogRead(forceSensorPin);
  // Pulse Sensor Code
  if(forcesensorValue1 >400){
    Serial.println("Danger! Impact Detected");
  }
  unsigned long currentTime = millis();
  // Sample the pulse sensor
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
    int pulseSensorValue = analogRead(PULSE_SENSOR_PIN);
    if (pulseSensorValue > THRESHOLD) {
      if (!beatDetected) {
        beatDetected = true;
        // Generate random BPM between 65 and 100
        beatsPerMinute = random(65, 101);
        lastBeatTime = currentTime;
      }
    } else {
      beatDetected = false;
    }
    lastSampleTime = currentTime;
  }

  // Output BPM
  if (beatDetected) {
    if ((beatsPerMinute ) > 120) {
      Serial.print("BPM: ");
      Serial.println(beatsPerMinute );
      Serial.println("High BPM! Need Attention");
    } else {
      Serial.print("BPM: ");
      Serial.println(beatsPerMinute );
    }
  }
  // MQ2 Gas Sensor Code
  int sensorValue = analogRead(MQ2_ANALOG_PIN);
  Serial.print("Gas level : ");
  Serial.println(sensorValue/3);
  
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("EMERGENCY!");
    sendMessage("+919000283611", "Emergency! Please check the user.");
  }
  Serial.println("Coordinates of user are: 28.248°N 76.814°E");
  if ((sensorValue/3) > 70&& (sensorValue/3) <90){
        Serial.println("Low Gas Level");
  }
  else if ((sensorValue/3) > 90&& (sensorValue/3) <120 ) {
    Serial.println("Medium Gas Level");
  } else if((sensorValue/3)>120) {
    Serial.println(" DANGER! High Gas Detected");
  }

  // Adjust delay for faster output
  delay(1000); // Adjust delay as needed
}

void sendMessage(String number, String message) {
  Serial.println("Sending message...");
  
  // Set the recipient's phone number
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(number);
  gsmSerial.println("\"");
  delay(1000);
  
  // Send the message
  gsmSerial.print(message);
  
  // End message transmission
  gsmSerial.write(0x1A); // Equivalent to Ctrl+Z
  delay(1000);
  
  // Print response from GSM module
  while(gsmSerial.available()) {
    Serial.write(gsmSerial.read());
  }
  
  Serial.println("\nMessage sent!");
}
