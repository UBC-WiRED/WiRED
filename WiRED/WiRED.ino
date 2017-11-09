 /*  
 * WiRED 
 * November 9th, 2017
 *  
 */

const int numReadings = 10;

int readingsA0[numReadings];
int readingsA1[numReadings];
int readingsA2[numReadings];
int readingsA3[numReadings];

int readIndex = 0;

int total0 = 0;
int total1 = 0;
int total2 = 0;
int total3 = 0;

int avg0 = 0;
int avg1 = 0;
int avg2 = 0;
int avg3 = 0;

int r0 = 0;
int r1 = 0;
int r2 = 0;
int r3 = 0;



int x = 0;                              // a place to hold pin values
int ledpin = 13;

int gPin;

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
  


  for(int pin = 0 ; pin<=3; pin++){
    gPin = pin;
    switch(pin){
      case 0: 
          avg0 = smoothing(pin, &total0, readingsA0, r0);
          sendValue(avg0);
          r0++; 
        break;
      case 1: 
           avg1 = smoothing(pin, &total1, readingsA1, r1);
          sendValue(avg1);
          r1++;
        break;
      case 2: 
          avg2 = smoothing(pin, &total2, readingsA2, r2);
          sendValue(avg2);
          r2++;
        break;
      case 3: 
          avg3 = smoothing(pin, &total3, readingsA3, r3);
          sendValue(avg3);
          r3++;
        break;
      default:
        break;
        
    }


    Serial.println();                 // Send a carriage returnt to mark end of pin data. 
    delay (50);                        // add a delay to prevent crashing/overloading of the serial port

  
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


void sendValue (int avg){              // function to send the pin value followed by a "space". 

     Serial.print(avg);
     Serial.write(32);
 
}

int smoothing(int inputPin, int* total, int readings[], int readIndex){
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
  *total = *total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  *total = *total + readings[readIndex];
  // advance to the next position in the array:

  
 
  // calculate the average:
  int average = *total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  

}



