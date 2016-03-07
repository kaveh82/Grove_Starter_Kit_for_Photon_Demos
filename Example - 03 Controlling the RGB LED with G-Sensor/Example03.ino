// -----------------------------------
// Example - 03: Controlling the RGB LED with Acceleration Sensor
// -- Modified by Kaveh Hadjari
// -----------------------------------
#include "application.h"
#include "math.h"
#include "MMA7660.h"
#include "ChainableLED.h"

MMA7660 accelemeter;

#define NUM_LEDS  1
ChainableLED leds(D4, D5, NUM_LEDS);

// This routine runs only once upon reset
void setup()
{
  accelemeter.init();                           // initialize the g-sensor
  leds.setColorRGB(0, 0, 0, 0);                 // turn down the LED
}

int i = 0;

//acceleration data 
MMA7660_ACC_DATA accData;

//acceleration values for each axis is stored in array with historic values
//with the last 5 readings, this is used for averaging a value and
//getting a smooth transition between color changes 
float axh[5], ayh[5], azh[5];

//the above historic acceleration values are averaged and finally normalized into the following variables that 
//constitutes as a unit vector
float ax, ay, az;

//the above unit vector is multiplied by 255 and stored in the following variables (x = red, y = green, z = blue)
//giving us a RGB-color value representing the acceleration vector in relation to the orientation of the accelorometer board. 
//When the accelerometer board is still it still outputs a value != 0, but this is due to gravitation, and so
//a still horizontal oriented board will cause a blue led color as z-value is then rougly 255.
uint8_t x, y, z;

//used to store magnitude of the acceleration vector and to calculate the unit vector
float magn = 1.0;

// This routine loops forever
void loop()
{

  //read acceleration from accelerometer
  accelemeter.getAcceleration(&accData);

//  if(i % 20 == 0) {
//    debug3DPointFloat("pre: x = %f, y = %f, z = %f", accData.x.g, accData.y.g, accData.z.g);
//  }

  //rotate array to make space for a new value
  leftRotate(axh, 1, 5);
  leftRotate(ayh, 1, 5);
  leftRotate(azh, 1, 5);
  
  //add new value at end of array
  axh[4] = accData.x.g;
  ayh[4] = accData.y.g;
  azh[4] = accData.z.g;
  
  //get average of the 5 latest accelaration values
  ax = avg(axh, 5);
  ay = avg(ayh, 5);
  az = avg(azh, 5);
  
  //get magnitude of acceleration
  magn = sqrtf(ax * ax + ay * ay + az * az);
  
  //if magn is tiny set it to 1.0, to ignore edge cases and prevent divide by zero
  if(magn < 0.0001)
    magn = 1.0;
  
  //calculate unit vector
  ax = ax / magn;
  ay = ay / magn;
  az = az / magn;

  //multiply by max rgb values
  x = std::min(abs(ax * 255), 255);
  y = std::min(abs(ay * 255), 255);
  z = std::min(abs(az * 255), 255);
  
//  if(i % 20 == 0) {
//    debug3DPoint("post: x = %d, y = %d, z = %d", x, y, z);
//  }

  leds.setColorRGB(0, x, y, z);                 // write LED data
  
  delay(100);

  i++;
  
}

/*Function to left rotate arr[] of size n by d*/
void leftRotate(float arr[], int d, int n)
{
  int i;
  for (i = 0; i < d; i++)
    leftRotatebyOne(arr, n);
}
 
void leftRotatebyOne(float arr[], int n)
{
  int i;
  float temp;
  temp = arr[0];
  for (i = 0; i < n-1; i++)
     arr[i] = arr[i+1];
  arr[i] = temp;
}

float avg(float arr[], int n)
{
  int i;
  float temp = arr[0];
  
  for (i = 0; i < n-1; i++)
     temp += arr[i+1];
     
  return temp / n;
  
}

/* following methods are used to publish debug messages to the particle.io dashboard  */
void debug3DPointFloat(String message, float value1, float value2, float value3)
{
    char msg [50];
    sprintf(msg, message.c_str(), value1, value2, value3);
    Particle.publish("DEBUG", msg);
}

void debug3DPoint(String message, int value1, int value2, int value3)
{
    char msg [50];
    sprintf(msg, message.c_str(), value1, value2, value3);
    Particle.publish("DEBUG", msg);
}
