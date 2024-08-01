/*
  Hardware Connections MAX30102 - LOLIN C3 MINI:
  - VIN = VCC
  - GND = GND
  - SDA = 8 (or SDA)
  - SCL = 10 (or SCL)

  By Melina Soula
*/

#include <DFRobot_MAX30102.h>
#include <Wire.h>
#include <Adafruit_GFX.h> // OLED libraries
#include <Adafruit_SSD1306.h>

DFRobot_MAX30102 particleSensor;

int32_t SPO2; // SPO2 value
int8_t SPO2Valid; // Flag to indicate if SPO2 calculation is valid
int32_t heartRate; // Heart rate value
int8_t heartRateValid; // Flag to indicate if heart rate calculation is valid
int32_t fingerFlag = 0; // Flag to check if the finger is placed on the sensor

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declaring the display object

static const unsigned char PROGMEM logo_bmp[] = {
  0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00
};

/*
Macro definition options in sensor configuration
sampleAverage: SAMPLEAVG_1 SAMPLEAVG_2 SAMPLEAVG_4
               SAMPLEAVG_8 SAMPLEAVG_16 SAMPLEAVG_32
ledMode:       MODE_REDONLY  MODE_RED_IR  MODE_MULTILED
sampleRate:    PULSEWIDTH_69 PULSEWIDTH_118 PULSEWIDTH_215 PULSEWIDTH_411
pulseWidth:    SAMPLERATE_50 SAMPLERATE_100 SAMPLERATE_200 SAMPLERATE_400
               SAMPLERATE_800 SAMPLERATE_1000 SAMPLERATE_1600 SAMPLERATE_3200
adcRange:      ADCRANGE_2048 ADCRANGE_4096 ADCRANGE_8192 ADCRANGE_16384
*/

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the sensor
  while (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
    delay(1000);
  }

  // Configure the sensor
  particleSensor.sensorConfiguration(
    /*ledBrightness=*/70,
    /*sampleAverage=*/SAMPLEAVG_4,
    /*ledMode=*/MODE_MULTILED,
    /*sampleRate=*/SAMPLERATE_100,
    /*pulseWidth=*/PULSEWIDTH_411,
    /*adcRange=*/ADCRANGE_16384
  );

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }
  display.display();
  display.clearDisplay();
}

void loop() {

  particleSensor.heartrateAndOxygenSaturation(&SPO2, &SPO2Valid, &heartRate, &heartRateValid);
  if(SPO2Valid == 1 && heartRateValid == 1){
    if (fingerFlag == 0) {
      for (int i = 16; i >= 1; i--) {  // Loop from 4 down to 1
          display.clearDisplay();
          display.setCursor(30, 6);
          display.setTextSize(1);
          display.println("Processing..."); 
          display.setTextSize(3);
          display.setTextColor(WHITE);
          display.setCursor(60, 30);
          display.println(i);  // Print the countdown number
          display.display();    // Update the display
          delay(1000);         // Wait for 1 second
        }
      fingerFlag = 1; // Set fingerFlag to 1 to indicate the loop has executed
    }
        if (SPO2Valid == 1 ){
        display.clearDisplay();
        display.drawBitmap(10, 19, logo_bmp, 32, 32, WHITE);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(60, 5);
        display.setTextSize(1);
        display.println("SPO2:");
        display.setCursor(60, 30);
        display.setTextSize(2);
        display.print(SPO2, DEC);
        display.println("%");
        display.display();

        delay(3000);}
        
        if (heartRateValid == 1){
        display.clearDisplay();
        display.drawBitmap(10, 19, logo_bmp, 32, 32, WHITE);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(60, 5);
        display.setTextSize(1);
        display.println("Pulses:");
        display.setCursor(60, 30);
        display.setTextSize(2);
        display.print(heartRate, DEC);
        display.setTextSize(1);
        display.println(" BPM");
        display.display();

        delay(3000);
        }
  }else {
    fingerFlag = 0;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(35, 6);
    display.println("No Finger?"); 
    display.setCursor(31, 21);
    display.println("Please Place");
    display.setCursor(35, 36);
    display.println("your Finger ");
    display.setCursor(37, 51);
    display.println("and wait! ");

    display.display();
  }
}
