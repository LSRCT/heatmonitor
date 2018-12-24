#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

int safetyCheck = 1;
int counter = 0;

// debug:
// 0: nothing is printed
// 1: light debug
// 2: verbose debug
int debug = 0;

const float onTemp = 50;
const float offTemp = 60;

const int relayPin = 13;
const int groundPin = 12;
const int buttonPin = 11;
const int sensorPin = A1;
const int ref = A0;

int avgnumber = 10;
float temparray[10] = {0}; 

// measured value of series resistor
const float rSerie = 975.0;

float vCC = 5.0;
const float v33 = 3.276;

const float alphaPt = 0.003851;
const float r0Pt = 1000.0;
// value substraced from rPt before calculations
// use this to compensate for different length cables
const float rcorrect = 19.0;

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
  // put Vcc to correct value
  setvCC();
  
}

void loop() {
  if(counter>9){counter = 0;}
  setvCC();
  float* temp = getTemp();
  if(debug){
    Serial.print("temp: ");
    Serial.println(temp[0]);
  }
  //checks what to do with the relay
  temp[0] = tempAvg(temp[0], counter);
  if(debug){
    Serial.print("AVGtemp: ");
    Serial.println(temp[0]);
  }
  putRelay(temp[0]);
  showValues(temp);
  delay(100);
  counter++;
}

float tempAvg(float temp, int counter){
  temparray[counter] = temp;
  float tempsum = 0.0;
  float avgsum = 0.0;
  for(int i = 0; i < 10; i++){
    tempsum = tempsum + temparray[i];
    }
  avgsum = tempsum/10.0;
  return avgsum;
  }
// function to put the relay according to set temps
void putRelay(float temp){
  if(safetyCheck){
    if(temp<=onTemp){
      digitalWrite(relayPin, LOW);
      }
    if(temp>=offTemp){
      digitalWrite(relayPin, HIGH);
      }
  } else {digitalWrite(relayPin, HIGH);}
}

void setvCC(){
  vCC = 2*(analogRead(ref)/1024.0)*v33;
  }

float* getTemp(){
  float rPt;
  float vPt;
  static float temp[2];
  digitalWrite(groundPin, LOW);
  delay(50);
  vPt = (analogRead(sensorPin)/1024.0)*v33;
  digitalWrite(groundPin, HIGH);
  rPt = ((rSerie)/((vCC/vPt)-1))-rcorrect;
  //Serial.println(rPt);
  temp[1] = rPt;
  temp[0] = (rPt-r0Pt)/(alphaPt*r0Pt);
  if(rPt < 950.0){safetyCheck = 0;}
  if(debug && safetyCheck == 0){
    Serial.println("SafetyCheck failed!");
  }
  return temp;
}

void showValues(float* temp){
  if(safetyCheck){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor((126/2)-11*2,0);
    display.print(round(temp[0]));
    display.write(247);
    display.println(F("C"));
  //  display.print(round(temp[1]));
  //  display.write(233);
    display.setTextSize(1);
    display.print(F("ON: "));
    display.setTextSize(2);
    display.print(round(onTemp));
    display.setCursor((126/2),16);
    display.setTextSize(1);
    display.print(F("OFF: "));
    display.setTextSize(2);
    display.print(round(offTemp));
    display.display();
  } 
  else {
    display.setTextSize(2);
    display.clearDisplay();
    display.setCursor((126/2)-(11*2+6),0);
    display.println(F("ERROR"));
    display.setTextSize(1);
    display.print(F("Check sensors and restart."));
    display.display();
  }
}

