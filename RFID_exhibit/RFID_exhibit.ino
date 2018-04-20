

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Time.h>
#include <TimeAlarms.h>

#define RST_1_PIN     4
#define RST_2_PIN     10
#define RST_3_PIN     5

#define SS_1_PIN      53   
#define SS_2_PIN      13    
#define SS_3_PIN      49   
     

#define numGates   3

//uncomment this line if using a Common Anode LED
#define COMMON_ANODE

//define a structure for a color
struct Color{
  int red;
  int green;
  int blue;
};

//set the pins that will be connected to the Arduino for rgb values
int redPins[] = {30, 31, 32};
int greenPins[] = {36, 37, 38};
int bluePins[] = {42, 43, 44};

int servoPins[] = {22, 23, 24};

//initialize a counter to track game progress
int gateNumber = 0;
int colorNumber = 0;

//define the colors that will be used in the game and store them in an array to be randomly sampled
struct Color red = {255, 0, 0};
struct Color green = {0, 255, 0};
struct Color blue = {0, 0, 255};
struct Color purple = {255, 0, 255};
struct Color aqua = {127, 255, 212};

Color colors[5] = {red, green, blue, purple, aqua};

//match colors to specific cards by using their UID
byte startTag = 174;
byte redCard = 74;
byte greenCard = 55;
byte blueCard = 74;
byte purpleCard = 55;
byte aquaCard = 74;

byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN};
byte rstPins[] = {RST_1_PIN, RST_2_PIN, RST_3_PIN};

MFRC522 rfid[numGates];   // Create MFRC522 instance.
Servo myServo[numGates];

int randomIndex;
int previousIndex = 100; //arbitrary index to make sure first color is random
 
void setup()
{
  Serial.begin(9600);
  while (!Serial);
  
  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < numGates; reader++) {
    rfid[reader].PCD_Init(ssPins[reader], rstPins[reader]); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    rfid[reader].PCD_DumpVersionToSerial();

    //initialize each LED light
    pinMode(redPins[reader], OUTPUT);
    pinMode(greenPins[reader], OUTPUT);
    pinMode(bluePins[reader], OUTPUT);
    setColor(0, 0, 0, reader);

    //initialize each servo and set them to the closed position
    myServo[reader].attach(servoPins[reader]);
    myServo[reader].write(160);
  } 
  
  randomSeed(analogRead(0));

  gateNumber = 0;
  colorNumber = 0;
}
 
void loop()
{
  if(gateNumber >= numGates){
    gateNumber = 0;
    colorNumber = 0;
  }

  //randomly select and set color
  randomIndex = random() % 5;
  //pick new colors until it is different from the previous one chosen
  while(randomIndex == previousIndex){
    randomIndex = random() % 5;
  }
  previousIndex = randomIndex;

  Color randColor = colors[randomIndex];
  setColor(randColor.red, randColor.green, randColor.blue, gateNumber);
  // Look for new cards
  Serial.print("Reading gate ");
  Serial.println(gateNumber);
  while(!readRFID(gateNumber));
  

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
  //purple
  else if(randomIndex == 3){
    myCard = purpleCard;
  }
  //aqua
  else{
    myCard = aquaCard;
  }
  
  //store the value of the card that was just scanned
  byte *card = rfid[gateNumber].uid.uidByte;

  //print the value of the scanned card
  Serial.print("Card: ");
  Serial.println(*card);

  //if the card is incorrect, keep scanning
  while(*card != myCard){
    Serial.println("Wrong Card!");
    //one second penalty
    delay(1000);
    Serial.print("Reading gate ");
    Serial.println(gateNumber);
    
    //wait for a card to be scanned
    while(!readRFID(gateNumber));
      
    card = rfid[gateNumber].uid.uidByte;
    Serial.print("Card: ");
    Serial.println(*card);
  }
  
  setColor(0, 0, 0, gateNumber);
  colorNumber++;

  //after 2 colors, move to the next gate
  if(colorNumber == 2){
    openGate();
    delay(2000);
  }
  
  if(gateNumber >= numGates){
    gameOver();
  }
}

void gameOver(){
  Serial.println("Game Over! Scan the start card at reader 1 to reset.");
  for(int gate = 0; gate < numGates; gate++){
    setColor(0, 0, 0, gate);
    myServo[gate].detach();
  }
  Serial.println("Reading...");
  while(!readRFID(0));
  
  //store the value of the card that was just scanned
  byte *card = rfid[0].uid.uidByte;

  //print the value of the scanned card
  Serial.print("Card: ");
  Serial.println(*card);

  //if the card is incorrect, keep scanning
  while(*card != startTag){
    Serial.println("Not the Start Tag!");
    Serial.println("Reading...");
    
    //wait for a card to be scanned
    while(!readRFID(0));
      
    card = rfid[0].uid.uidByte;
    Serial.print("Card: ");
    Serial.println(*card);
  }
  setup();
}
 
void setColor(int red, int green, int blue, int gate)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPins[gate], red);
  analogWrite(greenPins[gate], green);
  analogWrite(bluePins[gate], blue);  
}

int readRFID(uint8_t gate){

  // Look for new cards
  if ( ! rfid[gate].PICC_IsNewCardPresent())
    return 0;

  // Verify if the NUID has been readed
  if ( ! rfid[gate].PICC_ReadCardSerial())
    return 0;
    
   rfid[gate].PICC_HaltA(); // Stop reading
   rfid[gate].PCD_StopCrypto1();
   return 1;
}

void openGate(){
    myServo[gateNumber].write(80);
    gateNumber++;
    colorNumber = 0;
}


