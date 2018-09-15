#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include "MultipleDS18B20.h"
#include "ToEEPROMSaver.h"
#include "MySQL.h"

// Below variables are general global variables
uint8_t socketNumber;
const char *ssidSoftAP = "ESP8266 Access Point"; // The name of the Wi-Fi network that will be created
const char *passwordSoftAP = "";   // The password required to connect to it, leave blank for an open network

bool connected = false;
String tmp;

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);               // Create a Websocket server


void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:
      // Reset the control for sending to idle to allow for web server to respond.
      Serial.printf("[%u] Disconnected!\n", num);
      connected = false;
      yield();
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        delayInMillis = 1000;
        yield();
        socketNumber = num;
        break;
      }

    case WStype_TEXT:
      if (payload[0] == '#') {
        Serial.printf("[%u] get Text: %s\n", num, payload);

        if (payload[1] == 't') {
          tmp = "";
          for (int i = 2; isDigit(payload[i]); i++) {
            tmp += (char)payload[i];
          }
          delayInMillis = tmp.toInt();
        }

        if (payload[1] == 'r') {
          tmp = "";
          for (int i = 2; isDigit(payload[i]); i++) {
            tmp += (char)payload[i];
          }
          resolution = tmp.toInt();
          setResolutions();
        }

        if (payload[1] == 'p') {
          tmp = "";
          for (int i = 2; payload[i] != NULL; i++) {
            tmp += (char)payload[i];
          }
          Serial.println(tmp);
          cleanEEPROM();
          saveToEEPROM(tmp);
        }

        if (payload[1] == 's') {
          if (payload[2] == '1')
            connected = true;
          if (payload[2] == '0')
            connected = false;
        }

        if (payload[1] == 'g') {
          readFromEEPROM();
          webSocket.sendTXT(socketNumber, output);
        }

        yield();
      }
      break;

    case WStype_ERROR:
      Serial.printf("Error [%u] , %s\n", num, payload);
      yield();
  }
}


String getContentType(String filename) {
  yield();
  if (server.hasArg("download"))      return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html"))return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".json")) return F("text/json");
  else if (filename.endsWith(".js"))  return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz"))  return "application/x-gzip";
  else if (filename.endsWith(".svg")) return "image/svg+xml";
  else if (filename.endsWith(".otf")) return F("application/x-font-opentype");
  else if (filename.endsWith(".eot")) return F("application/vnd.ms-fontobject");
  else if (filename.endsWith(".woff")) return F("application/x-font-woff");
  else if (filename.endsWith(".woff2")) return F("application/x-font-woff2");
  else if (filename.endsWith(".ttf")) return F("application/x-font-ttf");
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);

  if (path.endsWith("/")) {
    path += "index.html";
  }

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  Serial.println("PathFile: " + pathWithGz);

  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  yield();
  return false;
}

void setupWiFiSoftAp() {
  WiFi.disconnect();
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_AP);
  Serial.println('\n');
  WiFi.softAP(ssidSoftAP, passwordSoftAP);
  Serial.print("Access Point \"");
  Serial.print(ssidSoftAP);
  Serial.println("\" started");
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
}

void stringToIpByteArray(String str, byte* arry) {
  String tmp = "";
  byte counter = 0;
  for (int j = 0; j < str.length(); j++) {
    if (str[j] == '.') {
      arry[counter] = tmp.toInt();
      counter++;
      tmp = "";
    } else {
      tmp += str[j];
    }
  }
  arry[counter++] = tmp.toInt();
}

void setupWiFiStationMode2() {

  String ssid;
  String pass;
  String mySqlServerIp;
  String espIp;
  String espGateway;
  String espSubnet;

  byte mySqlServerIpByte[4];
  byte espIpByte[4];
  byte espGatewayByte[4];
  byte espSubnetByte[4];

  readFromEEPROM();

  int i;
  for (i = 0; output[i] != ','; i++) {
    ssid += output[i];
  }

  i++;
  for (i; output[i] != ','; i++) {
    pass += output[i];
  }

  i++;
  for (i; output[i] != ','; i++) {
    mySqlServerIp += output[i];
  }
  stringToIpByteArray(mySqlServerIp, mySqlServerIpByte);
 
  i++;
  for (i; output[i] != ','; i++) {
    espIp += output[i];
  }
  stringToIpByteArray(espIp, espIpByte);


  i++;
  for (i; output[i] != ','; i++) {
    espGateway += output[i];
  }
  stringToIpByteArray(espGateway, espGatewayByte);

  i++;
  for (i; output[i] != NULL; i++) {
    espSubnet += output[i];
  }
  stringToIpByteArray(espSubnet, espSubnetByte);

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("PASS: ");
  Serial.println(pass);
  Serial.print("mySqlServerIp: ");
  Serial.println(mySqlServerIp);
  Serial.print("espIp: ");
  Serial.println(espIp);
  Serial.print("espGateway: ");
  Serial.println(espGateway);
  Serial.print("espSubnet: ");
  Serial.println(espSubnet);

  server_addr = mySqlServerIpByte;

  WiFi.disconnect();
  WiFi.config(espIpByte, espGatewayByte, espSubnetByte);
  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid.c_str(), pass.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connect to http://");
  Serial.println(WiFi.localIP());
}

