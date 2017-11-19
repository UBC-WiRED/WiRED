/*
 * MKR1000_UDP_Multiread
 * Base Source: https://www.arduino.cc/en/Tutorial/Wifi101WiFiUdpSendReceiveString
 * Editor: (Kyung)Jin Han
 * Description: This code takes the values coming from ADC ports (A1-6), encode them in OSC bytes.
 *              OSC data is then sent to the remote computer in the same network as MKR1000 by UDP.
 * Library: Modified WiFi101 library (includes OSC objects).
 */
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "OSCMessage.h"

//************************************************************
int status = WL_IDLE_STATUS;

int keyIndex = 0;

//Max,msp computer must be in the same network as below
char ssid[] = "Hello World";
char pass[] = "laptoporchestra491";
unsigned int localPort = 3001;

char packetBuffer[255];
char ReplyBuffer[] = "acknowledge";

//create new osc message
OSCMessage msg;
WiFiUDP Udp;

int x = 0;

//microcontroller WiFi setup
IPAddress IP(198,168,0,119);


//************************************************************setup
void setup() {
  Serial.begin(9600);

  //Comment below when powered by battery.
  while (!Serial) {
    // wait for serial port to connect.
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WIFI SHIELD NOT PRESENT");
    return; // don't continue
  }

  connectToWifi();
}


//************************************************************loop
void loop() {

  if ( status != WL_CONNECTED){
    Serial.println("Connection to SSID lost");
    //turn LED to Red.
    //try reconnecting
    connectToWifi();
  }
  
  int packetSize = Udp.parsePacket();
  
  //Purpose is to get IP address of MAX computer 
  if (packetSize)
  {
    //send a packet from MAX computer to initialize the UDP 
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    
    //record IP of MAX computer
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

    //turn LED on "Colour"
  }

  //start sending the value when we know the IP Address of the machine.
  if (Udp.remoteIP()){

    msg.beginMessage("sensors");
    for(int pin = 1; pin <= 2; pin++){
    x = analogRead(pin);
    msg.addArgInt32(x);


    Udp.beginPacket(Udp.remoteIP(), 8001);
    Udp.oscWrite(&msg);
    Udp.endPacket();
    msg.flush();
    

    
    }
  
    
    
    delay(50);
  }
}


// Initializing the printWifiStatus() Function. ************************************
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("WiFi Shield IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// Initializing the printWifiStatus() Function. ************************************
void connectToWifi() {
  
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    WiFi.config(IP);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}




