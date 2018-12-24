
const int sensorPin = A1;
cosnt int relayPin = 4;
const float rSerie = 975.0;
const float vCC = 4.7;
const float alphaPt = 0.0039083;
const float r0Pt = 1000.0;


void setup() {
  Serial.begin(9600);
  // set up the relay to be off on default for safety.
  pinMode(relayPin,OUTPUT);
  digitalWrite(relayPin, HIGH);
  // 3.3V analog reference
  analogReference(EXTERNAL);


}

void loop() {

  Serial.println(getTemp());
  delay(10);
}

float getTemp(){
  float rPt;
  float vPt;
  float temp;
  
  vPt = (analogRead(sensorPin)/1024.0)*3.27;
  rPt = ((rSerie)/((vCC/vPt)-1))-2;
  Serial.println(rPt);
  temp = (rPt-r0Pt)/(alphaPt*r0Pt);
  return temp;
  }
