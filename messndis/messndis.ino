#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

const int relayPin = 13;
const int groundPin = 12;

const int sensorPin = A1;
const int shuntPin = A0;

const float rSerie = 975.0;
const float vCC = 4.69;
const float alphaPt = 0.0039083;
const float r0Pt = 1000.0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  // 3.3V analog reference
  analogReference(EXTERNAL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.display();      // Show initial text

  // set up the relay to be off on default for safety.
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin, HIGH);
  // set up ground to be HIGH on default for sensor temperature.
  pinMode(groundPin,OUTPUT);
  digitalWrite(groundPin, HIGH);
  
}

void loop() {
  float* temp = getTemp();
  showValues(temp);
  //Serial.println(getTemp());
  if(temp[0] >= 26.0){
    digitalWrite(relayPin, LOW);
    } else {digitalWrite(relayPin, HIGH);}
  delay(500);
}

float* getTemp(){
  float rPt;
  float vPt;
  static float temp[2];
  digitalWrite(groundPin, LOW);
  delay(10);
  vPt = (analogRead(sensorPin)/1024.0)*3.253;
  digitalWrite(groundPin, HIGH);
  rPt = ((rSerie)/((vCC/vPt)-1))-2;
  //Serial.println(rPt);
  temp[1] = rPt;
  temp[0] = (rPt-r0Pt)/(alphaPt*r0Pt);
  return temp;
  }

void showValues(float* temp){
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(round(temp[0]));
  display.write(247);
  display.println(F("C"));
  display.print(round(temp[1]));
  display.write(233);
  display.display();
  }

