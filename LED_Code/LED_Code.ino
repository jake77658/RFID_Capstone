/*
Adafruit Arduino - Lesson 3. RGB LED
*/

struct Color{
  int red;
  int green;
  int blue;
};
 
int redPin = 11;
int greenPin = 10;
int bluePin = 9;

struct Color red = {255, 0, 0};
struct Color green = {0, 255, 0};
struct Color blue = {0, 0, 255};
struct Color yellow = {255, 255, 0};
struct Color purple = {80, 0, 80};
struct Color aqua = {0, 255, 255};

Color colors[6] = {red, green, blue, yellow, purple, aqua};

//uncomment this line if using a Common Anode LED
//#define COMMON_ANODE
 
void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  Serial.begin(9600);
}
 
void loop()
{
  int randomIndex = rand() % 6;
  Serial.print(randomIndex);
  Color randColor = colors[randomIndex];
  setColor(randColor.red, randColor.green, randColor.blue);  // red
  delay(5000);
//  delay(1000);
//  setColor(0, 255, 0);  // green
//  delay(1000);
//  setColor(0, 0, 255);  // blue
//  delay(1000);
//  setColor(255, 255, 0);  // yellow
//  delay(1000);  
//  setColor(80, 0, 80);  // purple
//  delay(1000);
//  setColor(0, 255, 255);  // aqua
//  delay(1000);
}
 
void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
