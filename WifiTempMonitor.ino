#include <FS.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>
#include "HelperFunctions.h"

String msg;
boolean enableDatabase = false;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT); // Set as output
  digitalWrite(LED_BUILTIN, HIGH); // Turn off LED

  pinMode(D7, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);

  Serial.begin(115200);
  delay(1000);
  SPIFFS.begin();

  if (digitalRead(D7)) {
    setupWiFiStationMode2();
    Serial.println("Station Mode");
  } else {
    setupWiFiSoftAp();
    Serial.println("Soft AP Mode");
  }

  server.on("/", HTTP_GET, []() {
    handleFileRead("/");
  });

  server.onNotFound([]() {                          // Handle when user requests a file that does not exist
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  webSocket.begin();                                // start webSocket server
  webSocket.onEvent(webSocketEvent);                // callback function

  server.begin();
  Serial.println("HTTP server started");

  setupMultipleDS18B20();

  if (digitalRead(D6)) {
    MySQLConnect();
    for (int i = 0; i < numberOfAddresses; i++) {
      MySQLSendCreate(strAdr[i]);
    }
  }
  
  yield();
}



void loop() {
  if (connected) {
    if (millis() - lastTempRequest >= delayInMillis) // waited long enough??
    {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println();
      Serial.print("Number of Devices found on bus = ");
      Serial.println(sensors.getDeviceCount());
      Serial.print("Getting temperatures... ");
      Serial.println();

      for (int i = 0; i < numberOfAddresses; i++) {
        Serial.print("Probe " + (String)i + " temperature is:   ");
        printTemperature(addresses[i]);
        Serial.println();
      }
      msg = "";
      msg += String(analogRead(A0));
      msg += ",";
      msg += String(numberOfAddresses);
      msg += ",";

      for (int i = 0; i < numberOfAddresses; i++) {
        msg += String(sensors.getTempC(addresses[i]));
        msg += ",";
      }

      for (int i = 0; i < numberOfAddresses; i++) {
        for (int j = 0; j < 8; j++) {
          msg += String(addresses[i][j]);
          msg += ",";
        }
      }

      webSocket.sendTXT( socketNumber, msg );
      
      if (digitalRead(D6)) {
        for (int i = 0; i < numberOfAddresses; i++) {
          MySQLSendInsert(strAdr[i], String(sensors.getTempC(addresses[i])) );
        }
      }
      
      sensors.requestTemperatures();
      lastTempRequest = millis();
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  server.handleClient();
  webSocket.loop();

}

