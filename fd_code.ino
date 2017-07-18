// DHT Sensor
#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t;
float h;


unsigned int reads;              // Int to hold how many read there have been
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int lightRelay = 7;
int fansRelay  = 6;

int loopCounter = 0;

void setup() {
  Serial.begin(19200);
  dht.begin();
  reads = 0;
  pinMode(lightRelay, OUTPUT);
  pinMode(fansRelay, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(lightRelay, LOW);
  digitalWrite(fansRelay, LOW);
  // !!Debug!!
  Serial.println("Starting");
  flash(5);
}

void loop() {
  loopCounter ++;

  // Saftey check to detact very high temps
  // to prevent fires.
  if (loopCounter > 10000) {
    t = dht.readTemperature();
    if (t > 75) {
      Serial.println("OverHeating - Shutting down and waiting for 1 minute");
      controlLight(HIGH);
      controlFans(HIGH);
     flash(12000);
    }
  }
  if (stringComplete) {
    
    if (inputString == "GetStatus") {
      // Read the TEMP and Humidity Sensor START
      t = dht.readTemperature();
      h = dht.readHumidity();
    
      if (isnan(h) || isnan(t)) {
        // Flash the on board LED 3 times
        return;
      }
      // Read the TEMP and Humidity Sensor END
      writeStatus();
      clearSerialBuffers();
      return;
    }

    if (inputString == "LightOff") {
      controlLight(HIGH);
      return;
    }

    if (inputString == "LightOn") {
      controlLight(LOW);
      return;
    }

    if (inputString == "FanOff") {
      controlFans(HIGH);
      return;
    }

    if (inputString == "FanOn") {
      controlFans(LOW);
      return;
    }

    errorOutput(1);
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void writeStatus() {
  reads++;
  // Status out START
  Serial.print("{t:");
  Serial.print(t);
  Serial.print(",h:");
  Serial.print(h);
  Serial.print(",light:");
  pinStatus(lightRelay);
  Serial.print(",fans:");
  pinStatus(fansRelay);
  Serial.print(",reads:");
  Serial.print(reads);
  Serial.println("}");
  // Status out END
  flash(4);
}

void writeRelayStatus() {
  Serial.println("ok");
}

void clearSerialBuffers() {
  inputString = "";
  stringComplete = false; 
}

void errorOutput(int number) {
   Serial.print("Error:");
   Serial.println(number);
   clearSerialBuffers();
}

void controlLight(bool mode) {
  digitalWrite(lightRelay, mode);
  writeRelayStatus();
  clearSerialBuffers();
  flash(3);
}

void controlFans(bool mode) {
  digitalWrite(fansRelay, mode);
  writeRelayStatus();
  clearSerialBuffers();
  flash(2);
}

void pinStatus(int pinNumber) {
  if ( ! digitalRead(pinNumber)) {
    Serial.print("on");
  } else {
    Serial.print("off");
  } 
}

void flash(int number) {
  for (int i=0; i < number; i++) {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }
}
