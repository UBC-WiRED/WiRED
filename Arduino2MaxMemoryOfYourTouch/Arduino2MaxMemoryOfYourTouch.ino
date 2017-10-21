 /*  
 *  Arduino2Max
 *  Send pin values from Arduino to MAX/MSP
 *  
 *  Arduino2Max.pde
 *  ------------  
 *  This version: .5, November 29, 2010
 *  ------------
 *  Copyleft: use as you like
 *  by Daniel Jolliffe
 *  Based on a sketch and patch by Thomas Ouellet Fredericks  tof.danslchamp.org
 *  
 */

const int numReadings = 10;

int readingsA0[numReadings];
int readingsA1[numReadings];
int readingsA2[numReadings];
int readingsA3[numReadings];

int readIndex = 0;

int total0, total1, total2, total3 = 0;



int x = 0;                              // a place to hold pin values
int ledpin = 13;

void setup()
{
  Serial.begin(9600);               // 115200 is the default Arduino Bluetooth speed
  digitalWrite(13,HIGH);              ///startup blink
  delay(600);
  digitalWrite(13,LOW);
  pinMode(13,INPUT);

  
  init_readings_array();
  
}



void loop()
{ 
  int avg0,avg1,avg2,avg3 = 0;


  for(int pin =0 ; pin<=3; pin++){
    switch(pin){
      case 0: 
          avg0 = smoothing(pin, total0, readingsA0);
          sendValue(avg0);
        break;
      case 1: 
           avg1 = smoothing(pin, total1, readingsA1);
          sendValue(avg1);
        break;
      case 2: 
          avg2 = smoothing(pin, total2, readingsA2);
          sendValue(avg2);
        break;
      case 3: 
          avg3 = smoothing(pin, total3, readingsA3);
          sendValue(avg3);
        break;
      default:
        break;
        
    }
  }

 readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  
      Serial.println();                 // Send a carriage returnt to mark end of pin data. 
      delay(10);                        // add a delay to prevent crashing/overloading of the serial port
      //sendValue(avg);

}

void init_readings_array(){
     // initialize all the readings to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readingsA0[thisReading] = 0;
      readingsA1[thisReading] = 0;
      readingsA2[thisReading] = 0;
      readingsA3[thisReading] = 0;
    }
}


void sendValue (int x){              // function to send the pin value followed by a "space". 
 Serial.print(x);
 Serial.write(32); 
}

double smoothing(int inputPin, int total, int readings[]){
/*
    int readings[numReadings];      // the readings from the analog input
               
    double total = 0;                  // the running total
    int average = 0;                // the average



  for(int readIndex = 0; readIndex < numReadings; readIndex ++ ) {

    
     // subtract the last reading:
      total = total - readings[readIndex];
      // read from the sensor:
      readings[readIndex] = analogRead(inputPin);
      // add the reading to the total:
      total = total + readings[readIndex];
      // advance to the next position in the array:
      readIndex = readIndex + 1;

  }
  

  // calculate the average:
  average = total / numReadings;

  return average;

*/
// subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
 
  // calculate the average:
  int average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1);        // delay in between reads for stability

}



