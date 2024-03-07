#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Servo.h>

#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '10'},
  {'4', '5', '6', '11'},
  {'7', '8', '9', '12'},
  {'15', '0', '14', '13'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; 
byte colPins[COLS] = {9, 8, 7, 6}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

#define SENSORPIN 11

#define SENSORPIN2 12 // changable 

int myarray[2]; // signal for checking trepassing or exiting 

int servoPin = 13;

Servo Servo1; 

int sensorState = 0, lastState=0;     // variable for reading the push button status

int sensorState2 = 0, lastState2=0;

int count = 0; // number of people trepassing

int max_num = 5;

void setup() {
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);   
  
  pinMode(SENSORPIN2, INPUT);  

  digitalWrite(SENSORPIN, HIGH); // turn on the pullup

  digitalWrite(SENSORPIN2, HIGH); // turn on the pullup

  lcd.init();  //initialize the lcd

  lcd.backlight();  //open the backlight

  Servo1.attach(servoPin); // Servo Motor

  myarray[0] = 0;
  myarray[1] = 0;
  
  Serial.begin(9600);
}

void loop(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  sensorState2 = digitalRead(SENSORPIN2);

  // changing the maximum number

  char current_input = customKeypad.getKey();

  int mynum = int(current_input)-48;

  Serial.print(mynum);

  if (mynum > 0){
  
    max_num = mynum;
  }

  // task for LCD Screen

  lcd.setCursor(0, 0);
  lcd.print("Maximum : ");

  lcd.setCursor(10, 0);
  lcd.print("    pp");

  lcd.setCursor(10, 0);
  lcd.print(max_num);

  lcd.setCursor(0, 1);
  lcd.print("Current : ");

  lcd.setCursor(10, 1);
  lcd.print("    pp");

  lcd.setCursor(10, 1);
  lcd.print(count);

  // task check sensor 1
    // check if the sensor beam 1 is broken
  if (sensorState && !lastState) {

    Serial.println("Unbroken");
  } 

  if (!sensorState && lastState) {

    Serial.println("Broken");

    if(myarray[0] == 0){

      myarray[0] = 1; // put 1 into first element if 2 hasn't been added yet
    
    }else if(myarray[0] == 2){
      
      myarray[1] = 1; // put in second element if 2 hasn't been added
    }
  }

  lastState = sensorState;

    // check if the sensor beam 2 is broken
  if (sensorState2 && !lastState2) {

    Serial.println("Unbroken2");
  }

  if (!sensorState2 && lastState2) {

    Serial.println("Broken2");
    
    if(myarray[0]==0){
    
      myarray[0] = 2; // put 1 into first element if 2 hasn't been added yet
    
    }else if (myarray[0] == 1){
      
      myarray[1] = 2;
    }  
  }
  
  lastState2 = sensorState2;

  if((myarray[0] == 1) && (myarray[1] == 2)) {

    count++;

    myarray[0] = 0;
    myarray[1] = 0; // reset the parameter array

    Serial.print(count);

    delay(500);

  }else if((myarray[0] == 2) && (myarray[1] == 1) && (count >= 1)){

    count--;

    myarray[0] = 0;
    myarray[1] = 0;

    Serial.println(count);

    delay(500);

  }else if((myarray[0] == 2) && (myarray[1] == 1) && (count == 0)){

    myarray[0] = 0;
    myarray[1] = 0;

    Serial.println(count);

    delay(500);
  }


  //for servo motor
  if(max_num <= count){

    Servo1.write(180); 
    delay(15);

  }else{

    Servo1.write(90); 
    delay(15);
  }

}
