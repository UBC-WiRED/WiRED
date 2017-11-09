/*
  Smoothing

  Reads repeatedly from an analog input, calculating a running average and
  printing it to the computer. Keeps ten readings in an array and continually
  averages them.

  The circuit:
  - analog sensor (potentiometer will do) attached to analog input 0

  created 22 Apr 2007
  by David A. Mellis  <dam@mellis.org>
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Smoothing
*/

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 10;
const int numReadIndexes = numReadings;
const int numPins = 2;

long readingsArray[numPins][numReadings];      // the readings from the analog input
long averageArray[numPins];

int readIndex = 0;              // the index of the current reading
//int total= 0;                  // the running total
//int average = 0;                // the average

//int inputPin = A1;

void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  // initialize all the readings to 0:
  for (int thisReadingArray = 0; thisReadingArray < numPins; thisReadingArray++) {
      for(int thisReading = 0; thisReading < numReadings; thisReading++) {
        readingsArray[thisReadingArray][thisReading] = 0;
      }
  }
}

void loop() {
  int temp = readIndex;
  
  smoothing(0, A1);
  
  readIndex = temp;
  
  smoothing(1, A2);

  Serial.println();

}


void smoothing(int index, int inputPin){
  
  readingsArray[index][readIndex] = analogRead(inputPin);

  readIndex = readIndex + 1;
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  long sum = 0;

  for(int i = 0; i< numReadings; i++){
    sum += readingsArray[index][i];
  }
  
  // calculate the average:
  averageArray[index] = sum / 10.0;
  
  // send it to the computer as ASCII digits
  
  sendValue(averageArray[index]);
 
  /* //debugging
   *  for(int i =0; i< numReadings; i++){
    sendValue(readingsArray[index][i]);
  }*/
  //Serial.print(sum/10.0);
  
  delay(50);        // delay in between reads for stability
}

void sendValue(int value){
  Serial.print(value);
  Serial.print(" ");
  //Serial.print(32);
}


