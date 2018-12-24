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
int debug = 2;

const float onTemp = 25;
const float offTemp = 27;

const int relayPin = 4;
const int relayPin2 = 1;
const int groundPin = 3;
const int buttonPin = 11;

const int sensorPin = A3;
const int sensorPin2 = A2;
const int sensorPin3 = A1;

const int ref = A7;

float temparray1[10] = {0}; 
float temparray2[10] = {0};
float temparray3[10] = {0};

// measured value of series resistor
const float rSerie = 981.0;
const float rSerie2 = 983.0;
const float rSerie3 = 984.0;

float vCC = 5.0;
const float v33 = 3.260;

const float alphaPt = 0.003851;
const float r0Pt = 1000.0;
// value substraced from rPt before calculations
// use this to compensate for different length cables
const float rcorrect = 2.0;

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
  float* temp = getTemp();
  if(debug){
    Serial.println("temp: ");
    Serial.println(temp[0]);
    Serial.println(temp[1]);
    Serial.println(temp[2]);
  }
  //checks what to do with the relay
  temp[0] = tempAvg(temp[0], counter, 1);
  temp[1] = tempAvg(temp[1], counter, 2);
  temp[2] = tempAvg(temp[2], counter, 3);
  if(debug>1){
    Serial.println("AVGtemp: ");
    Serial.println(temp[0]);
    Serial.println(temp[1]);
    Serial.println(temp[2]);
  }
  putRelay(temp);
  showValues(temp);
  delay(100);
  counter++;
}

float tempAvg(float temp, int counter, int sensor){
  float* temparrayspes;
  switch(sensor) {
    case 1: temparrayspes = temparray1; break;
    case 2: temparrayspes = temparray2; break;
    case 3: temparrayspes = temparray3; break;
    }
  temparrayspes[counter] = temp;
  float tempsum = 0.0;
  float avgsum = 0.0;
  for(int i = 0; i < 10; i++){
    tempsum = tempsum + temparrayspes[i];
    }
  avgsum = tempsum/10.0;
  return avgsum;
  }
// function to put the relay according to set temps
void putRelay(float* temp){
  if(safetyCheck){
    if(temp[0]<=onTemp){
      digitalWrite(relayPin, LOW);
      }
    if(temp[0]>=offTemp){
      digitalWrite(relayPin, HIGH);
      }
  } else {digitalWrite(relayPin, HIGH);}
}

void setvCC(){
  vCC = 2*(analogRead(ref)/1024.0)*v33;
  if(debug){
    Serial.println("vCC: ");
    Serial.println(vCC);
  }
  }

float* getTemp(){
  float rPt[3];
  float vPt[3];
  //ground level of digital LOW
  float vCorrect = 0.021; 
  int workingS = 0;
  static float temp[4];
  digitalWrite(groundPin, LOW);
  delay(50);
  setvCC();
  vPt[0] = (analogRead(sensorPin)/1024.0)*v33;
  vPt[1] = (analogRead(sensorPin2)/1024.0)*v33;
  vPt[2] = (analogRead(sensorPin3)/1024.0)*v33;
  digitalWrite(groundPin, HIGH);
  
  for(int i = 0; i < 3; i++){
    if(vPt[i]<(v33-0.2)){
      workingS++;
      }
    }
  switch(workingS){
    case 2: vCorrect = 0.045; break;
    case 3: vCorrect = 0.064; break;
    }
  vPt[0] = vPt[0]-vCorrect;
  vPt[1] = vPt[1]-vCorrect;
  vPt[2] = vPt[2]-vCorrect;
  rPt[0] = ((rSerie)/((vCC/vPt[0])-1))-rcorrect;
  rPt[1] = ((rSerie2)/((vCC/vPt[1])-1))-rcorrect;
  rPt[2] = ((rSerie3)/((vCC/vPt[2])-1))-rcorrect;
  temp[0] = (rPt[0]-r0Pt)/(alphaPt*r0Pt);
  temp[1] = (rPt[1]-r0Pt)/(alphaPt*r0Pt);
  temp[2] = (rPt[2]-r0Pt)/(alphaPt*r0Pt);
  if(rPt[0] < 950.0 || rPt[1] < 950.0 || rPt[2] < 950.0){safetyCheck = 0;}
  if(debug && safetyCheck == 0){
    Serial.println("SafetyCheck failed!");
  }
  if(debug){
    Serial.println("vPt: ");
    Serial.println(vPt[0]);
    Serial.println(vPt[1]);
    Serial.println(vPt[2]);
  }
  if(debug>1){
    Serial.println("rPt: ");
    Serial.println(rPt[0]);
    Serial.println(rPt[1]);
    Serial.println(rPt[2]);
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

