#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_PIN D1
byte resolution = 10;

OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature sensors(&oneWire);

String strAdr[64];
DeviceAddress addresses[64];
int numberOfAddresses = 0;

// for async non blocking run
unsigned long lastTempRequest = 0;
int  delayInMillis = 0;
int  idle = 0;

void discoverOneWireDevices(void);

void setResolutions() {
   for (int i = 0; i < numberOfAddresses; i++) {
    sensors.setResolution(addresses[i], resolution);
  }
}

void setupMultipleDS18B20(){

  discoverOneWireDevices();
  sensors.begin();
  setResolutions();
  
  // for async non blocking run
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  delayInMillis = 1000;
  lastTempRequest = millis(); 
}


void printTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Error getting temperature  ");
  } else {
    Serial.print("C: ");
    Serial.print(tempC);
    Serial.print(" F: ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
  }
}

void discoverOneWireDevices(void) {
  byte i;
  byte addr[8];

  Serial.print("Looking for 1-Wire devices...\n\r");
  while (oneWire.search(addr)) {
    Serial.print("\n\r\n\rFound \'1-Wire\' device with address:\n\r");

    for ( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }

    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n\r");
      return;
    }

    for (i = 0; i < 8; i++) {
      addresses[numberOfAddresses][i] = addr[i];
      if (addresses[numberOfAddresses][i] < 16) {
        strAdr[numberOfAddresses] += "0";
      }
      strAdr[numberOfAddresses] += String(addresses[numberOfAddresses][i], HEX);
    }
    
    numberOfAddresses++;
  }

  Serial.println();
  Serial.println("Addresses found: " + (String)numberOfAddresses);
  Serial.print("Done");

  oneWire.reset_search();
}
