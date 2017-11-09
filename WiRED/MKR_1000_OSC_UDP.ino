
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "OSCMessage.h"

//************************************************************
int status = WL_IDLE_STATUS;

char ssid[] = "Hello World";
char pass[] = "laptoporchestra491";
int keyIndex = 0;

unsigned int localPort = 3001;

char packetBuffer[255];
char ReplyBuffer[] = "acknowledge";

WiFiUDP Udp;

//create new osc message
OSCMessage msg;

byte dummieIp[]  = { 192, 168, 0, 2 };


// Initializing the printWifiStatus() Function.
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//************************************************************
void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    return; // don't continue
  }
  
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}



//************************************************************
void loop() {
  int packetSize = Udp.parsePacket();
  
  //Purpose is to get IP address of Mac 
  if (packetSize)
  {
    //send a packet from Mac computer to initialize the UDP 
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    
    //record IP of Mac computer
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();

  }

  //start sending the value when we know the IP Address of the machine.
  if (Udp.remoteIP()){
    msg.setAddress(dummieIp, 8001);
    //msg.beginMessage("optional begin message");
    msg.addArgInt32(analogRead(A1));
    //msg.addArgFloat(v2);
    //msg.addArgString(inputVal);

    Udp.beginPacket(Udp.remoteIP(), 8001);
    Udp.oscWrite(&msg);
    Udp.endPacket();
    msg.flush();

    delay(500);
  }
}

