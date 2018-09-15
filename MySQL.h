#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

IPAddress server_addr;          // MySQL server IP
char user[] = "esp8266";        // MySQL user
char password[] = "esp8266";    // MySQL password

char CREATE_SQL[] = "CREATE TABLE `esp8266`.`%s`(`id` INT NOT NULL AUTO_INCREMENT,`date` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,`temp` FLOAT NOT NULL,PRIMARY KEY (`id`)) ENGINE = InnoDB";
char INSERT_SQL[] = "INSERT into `esp8266`.`%s` values(NULL, NOW(), %s)";
char query[256];

WiFiClient client;
MySQL_Connection conn((Client *)&client);
WiFiServer srv(80);

void MySQLConnect() {
  Serial.println("Connecting to database...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  } else {
    Serial.println("Connection failed.");
    conn.close();
  }
}

void MySQLSendInsert(String into, String temp) {
  char copy[64];
  into.toCharArray(copy, 64);

  char copy1[64];
  temp.toCharArray(copy1, 64);

  sprintf(query, INSERT_SQL, copy, copy1);
  Serial.println("Recording data.");
  Serial.println(query);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
}

void MySQLSendCreate(String name) {
  char copy[64];
  name.toCharArray(copy, 64);
  sprintf(query, CREATE_SQL, copy);
  Serial.println("Recording data.");
  Serial.println(query);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
}

void MySQLSendQuery(char* sql) {
  sprintf(query, sql);
  Serial.println("Recording data.");
  Serial.println(query);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
}
