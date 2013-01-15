// Red alreat for Haojing deploying(and other things)
// ver 1.1.0
// originally composed by zhaojun
// modified by zhujun 2013-01-15

#include <SPI.h>
#include <Ethernet.h>

//toggle debug
#define DEBUG 0

EthernetClient client;

//deploy server
char server[] = "deploy.baixing.com";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 8, 222);
IPAddress dns_server(172, 16, 0, 1);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

//output pin can not use 11-13, cause w5100 use it for ethernet.
int output_pin = 8;

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 10 * 1000; //every 10 seconds

void setup() {
  if (DEBUG) Serial.begin(115200);
  
  Ethernet.begin(mac, ip, dns_server, gateway, subnet);

  if (DEBUG) Serial.print("Begin with: ");
  if (DEBUG) Serial.println(Ethernet.localIP());
  
  pinMode(output_pin, OUTPUT);

  //init set
  digitalWrite(output_pin, LOW);
}

void loop() {
  boolean is_deploying = false;
  
  //http response deal vars
  String currentLine = "";
  boolean currentLineIsBlank = true;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      currentLine += c;
      
      if (c == '\n' && currentLineIsBlank) {
        break;
      }
      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
        if (DEBUG) Serial.print(currentLine);
        if (DEBUG) Serial.println(currentLine.indexOf("Someone is doing:"));
        if (currentLine.indexOf("Someone is doing:") > -1) {
          is_deploying = true;
        }

        currentLine = "";
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }

  if (is_deploying) {
    if (DEBUG) Serial.println("depolying");
    is_deploying = false;
    alarm();
    // may delay 120 sec for skip this depolying, not acurate
    //delay(120000);
  }

  if (!client.connected() && lastConnected) {
    if (DEBUG) Serial.println();
    if (DEBUG) Serial.println("disconnecting.");
    client.stop();
  client.flush();
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpRequest();
  }

  lastConnected = client.connected();
}

void httpRequest() {
  if (client.connect(server, 80)) {
    if (DEBUG) Serial.println("connecting...");
    client.println("GET / HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: deploy-monitor");
    client.println("Connection: close");
    client.println();

    lastConnectionTime = millis();
  } else {
    if (DEBUG) Serial.println("connection failed");
    if (DEBUG) Serial.println("disconnecting.");
    client.stop();
  }
}

void alarm() {
  if (DEBUG) Serial.println("alarm");
  digitalWrite(output_pin, HIGH);
  
  //10 sec alarm
  delay(10000);
  digitalWrite(output_pin, LOW);
}

