

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>


#define SS_PIN 10
#define RST_PIN 9

//uncomment this line if using a Common Anode LED
#define COMMON_ANODE

//define a structure for a color
struct Color{
  int red;
  int green;
  int blue;
};

//set the pins that will be connected to the Arduino for rgb values
int redPin = 7;
int greenPin = 6;
int bluePin = 5;

//initialize a counter to track game progress
int gateNumber = 0;

//define the colors that will be used in the game and store them in an array to be randomly sampled
struct Color red = {255, 0, 0};
struct Color green = {0, 255, 0};
struct Color blue = {0, 0, 255};
struct Color yellow = {255, 255, 0};
struct Color purple = {255, 0, 255};
struct Color aqua = {0, 255, 255};

Color colors[6] = {red, green, blue, yellow, purple, aqua};

//match colors to specific cards by using their UID
byte redCard = 142;
byte greenCard = 117;
byte blueCard = 117;
byte yellowCard = 142;
byte purpleCard = 117;
byte aquaCard = 142;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
Servo myServo;
 
void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  Serial.begin(9600);

  myServo.attach(4);

  myServo.write(10);

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  randomSeed(analogRead(0));

}
 
void loop()
{
  if(gateNumber == 6){
    gateNumber = 0;
  }

  myServo.write(10);
  //randomly select and set color
  int randomIndex = random() % 6;
  Color randColor = colors[randomIndex];
  setColor(randColor.red, randColor.green, randColor.blue);
  
  // Look for new cards
  Serial.println("Reading...");
  while (! readRFID());

  //create random variable to represent the correct card value for comparison
  byte myCard;
  //red
  if(randomIndex == 0){
    myCard = redCard;
  }
  //green
  else if(randomIndex == 1){
    myCard = greenCard;
  }
  //blue
  else if(randomIndex == 2){
    myCard = blueCard;
  }
  //yellow
  else if(randomIndex == 3){
    myCard = yellowCard;
  }
  //purple
  else if(randomIndex == 4){
    myCard = purpleCard;
  }
  //aqua
  else{
    myCard = aquaCard;
  }
  
  //store the value of the card that was just scanned
  byte *card = rfid.uid.uidByte;

  //print the value of the scanned card
  Serial.print("Card: ");
  Serial.println(*card);

  //if the card is incorrect, keep scanning
  while(*card != myCard){
    Serial.println("Wrong Card!");
    //one second penalty
    delay(1000);
    Serial.println("Reading...");
    
    //wait for a card to be scanned
    while(!readRFID());
      
    card = rfid.uid.uidByte;
    Serial.print("Card: ");
    Serial.println(*card);
  }

  gateNumber++;
  myServo.write(165);
  delay(5000);

  if(gateNumber >= 6){
    gameOver();
  }
}

void gameOver(){
  setColor(0, 0, 0);
  exit(0);
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

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

int readRFID(){
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return 0;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return 0;
    
   rfid.PICC_HaltA(); // Stop reading
   return 1;

}


