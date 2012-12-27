
#include <SPI.h>
#include <Ethernet.h>

EthernetClient client;

//deploy server
char server[] = "baixing.com";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 8, 222);
IPAddress dns_server(172, 16, 0, 1);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

//output pin can not use 11-13, cause w5100 use it for ethernet.
int output_pin = 8;

unsigned long lastConnectionTime = 0;
boolean lastConnected = false;
const unsigned long postingInterval = 10 * 1000; //every 6 seconds

void setup() {
  Serial.begin(9600);
  
  Ethernet.begin(mac, ip, dns_server, gateway, subnet);

  Serial.print("Begin with: ");
  Serial.println(Ethernet.localIP());
  
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

        if (currentLine.substring(0, 11) == "Deploy: ing") {
          is_deploying = true;
        }

        currentLine = "";
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }

  if (is_deploying) {
    Serial.println("depolying");
    is_deploying = false;
    alarm();
    //may delay 120 sec for skip this depolying
    //delay(120000);
  }

  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpRequest();
  }

  lastConnected = client.connected();
}

void httpRequest() {
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    client.println("GET / HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: deploy-monitor");
    client.println("Connection: close");
    client.println();

    lastConnectionTime = millis();
  } else {
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

void alarm() {
  Serial.println("alarm");
  digitalWrite(output_pin, HIGH);
  
  //10 sec alarm
  delay(10000);
  digitalWrite(output_pin, LOW);
}





