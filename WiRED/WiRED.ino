/*
 * WiRED Project
 * Base Source: https://www.arduino.cc/en/Tutorial/Wifi101WiFiUdpSendReceiveString
 * Author: UBC EECE Capstone Group 93
 * Description: This code takes the values coming from ADC pins (A1-6), encode them in OSC bytes.
 *              OSC data is then sent to the remote computer in the same network as MKR1000 by UDP.
 * Library: Modified WiFi101 library (includes OSC objects).
 */
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Ethernet.h>


#include "OSCMessage.h"

//************************************************************
//Constants
const int baud_rate = 9600;

//Values for measuring resistance
int raw= 0;
int Vin= 3.3;
float Vout= 0;
float R1= 3000;
float R2= 0;
float buffer= 0;



int status = WL_IDLE_STATUS;

int keyIndex = 0;

IPAddress ip(192, 168, 0, 119); 

//Max,msp computer must be in the same network as below
char ssid[] = "Hello World";
char pass[] = "laptoporchestra491";
unsigned int localPort = 3001;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char ReplyBuffer[] = "acknowledge";

//create new osc message
OSCMessage msg;
OSCMessage resp;
WiFiUDP Udp;

int pinReading = 0;

//Battery Pin 
const int batteryPin = 0;


//************************************************************setup
void setup() {


  pinMode(6,OUTPUT); //LED
  
  Serial.begin(baud_rate);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    return; // don't continue
  }


  connectToWifi();
}


//************************************************************loop
void loop() {

  if ( WiFi.status() != WL_CONNECTED){
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
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    String contents(packetBuffer);
 
    int len = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(contents);

    
    
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();

    //TODO turn LED on "Colour"

    if(contents == "battery"){
      checkBattery();
    }

    if(contents == "baudRate") {
      checkBaudRate();
    }

    if(contents == "connect") {
      sendConnectedMSG();
    }
  



    
  }

  //start sending the value when we know the IP Address of the machine.
  if (Udp.remoteIP()){

    msg.beginMessage("sensors");
    for(int pin = 1; pin <= 5; pin++){ 
      pinReading = analogRead(pin);
      if(pin == 5) {//pin 5 is for reading resistance see  http://www.circuitbasics.com/arduino-ohm-meter/
        raw = pinReading;
        buffer= raw * Vin;
        Vout= (buffer)/1024.0;
        buffer= (Vin/Vout) -1;
        pinReading= R1 * buffer;
        Serial.print("Vout: ");
        Serial.println(Vout);
        Serial.print("R2: ");
        Serial.println(pinReading);
      }
    
    
    msg.addArgInt32(pinReading);
    }
    //Serial.println(analogRead(3));
    
    sendUDP();
    delay(50);
  }
}

void sendUDP(){
    Udp.beginPacket(Udp.remoteIP(), 8001);
    Udp.oscWrite(&msg);
    Udp.endPacket();
    msg.flush();
}


// Initializing the printWifiStatus() Function. ************************************
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

void blinkLED() {
  for(int i = 0; i < 10; i++){
    digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);              // wait for 500ms
    digitalWrite(6, LOW);    // turn the LED off by making the voltage LOW
    delay(500);              // wait for 500ms  
  }
  
}

// Initializing the printWifiStatus() Function. ************************************
void connectToWifi() {

  digitalWrite(6, LOW);    // turn the LED off by making the voltage LOW
    
  // attempt to connect to WiFi network:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);

    //Config the ip address of this WM
    WiFi.config(ip);
    // wait 10 seconds for connection:
   //delay(10000);
    blinkLED();
    
  }

  digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  Serial.println(WiFi.localIP());
  
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}


void checkBattery(){
    
    float batteryVoltage = (analogRead(batteryPin)*(3.3*2/1023.0));
    float batteryLevel = 100 *(batteryVoltage - 3)/1.2;

    Serial.print("Battery voltage: ");
    Serial.println(batteryVoltage);
    Serial.print("Battery level: ");
    Serial.println(batteryLevel);
    
    OSCMessage batLevelMSG;
    batLevelMSG.beginMessage("battery");
    batLevelMSG.addArgFloat(batteryLevel);
    Udp.beginPacket(Udp.remoteIP(), 8001);
    Udp.oscWrite(&batLevelMSG);
    Udp.endPacket();
    batLevelMSG.flush();
  
  sendUDP();

}

void checkBaudRate() {
  
  Serial.print("Baud Rate: ");
  Serial.println(baud_rate);

  OSCMessage baudRateMSG;
  baudRateMSG.beginMessage("baudRate");
  baudRateMSG.addArgInt32(baud_rate);

 Udp.beginPacket(Udp.remoteIP(), 8001);
  Udp.oscWrite(&baudRateMSG);
  Udp.endPacket();
  baudRateMSG.flush();
  
  sendUDP();


}


void sendConnectedMSG() {

  OSCMessage connectedMSG;
  connectedMSG.beginMessage("wmmsgreceived");
  connectedMSG.addArgInt32(1);

  Udp.beginPacket(Udp.remoteIP(), 8001);
  Udp.oscWrite(&connectedMSG);
  Udp.endPacket();
  connectedMSG.flush();
  
  sendUDP();


}

