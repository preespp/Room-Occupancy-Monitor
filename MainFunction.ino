#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Servo.h>
#include <Keypad.h>
#include <stdlib.h>
#include <string.h>

const byte ROWS = 4; 
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', ':'},
  {'4', '5', '6', ';'},
  {'7', '8', '9', '<'},
  {'-', '0', '+', '='}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; 
byte colPins[COLS] = {9, 8, 7, 6}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

#define SENSORPIN 11

#define SENSORPIN2 12 // changable 

// for UART Communication with esp32
#define RXp2 0
#define TXp2 1

int myarray[2]; // signal for checking trepassing or exiting 

int servoPin = 13;

Servo Servo1; 

int sensorState = 0, lastState=0;     // variable for reading the push button status

int sensorState2 = 0, lastState2=0;

int count = 0; // number of people trepassing

int max_num = 5;

char mem_number[100];

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
  //Serial.begin(115200, SERIAL_8N1, RXp2, TXp2);
}

void loop(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  sensorState2 = digitalRead(SENSORPIN2);

  // convert ascii to number (off-set 48)
  int current_input = customKeypad.getKey()-48;

  if(current_input!=-48){
    if(current_input== -5){ // convert ascii of +1
      max_num++;
    }else if(current_input == -3 && max_num > 0){ // convert ascii of -1
      max_num--;
    }else{
      max_num = current_input;
    }
  }

  // task for LCD Screen

  lcd.setCursor(0, 0);
  lcd.print("Maximum : ");

  lcd.setCursor(10, 0);
  lcd.print("    pp");

  lcd.setCursor(11, 0);
  lcd.print(max_num);
  Serial.println("Hello World");
  // Serial.println(max_num);
  // Serial.println(" ");
  // Serial.println(count);
  // Serial.println(" ");

  lcd.setCursor(0, 1);
  lcd.print("Current : ");

  lcd.setCursor(10, 1);
  lcd.print("    pp");

  lcd.setCursor(11, 1);
  lcd.print(count);

  // task check sensor 1
    // check if the sensor beam 1 is broken
  //if (sensorState && !lastState) {
    //break;
    //Serial.println("Unbroken");
  //}

  if (!sensorState && lastState) {

    // Serial.println("Broken");

    if(myarray[0] == 0){

      myarray[0] = 1; // put 1 into first element if 2 hasn't been added yet
    
    }else if(myarray[0] == 2){
      
      myarray[1] = 1; // put in second element if 2 hasn't been added
    }
  }

  lastState = sensorState;

    // check if the sensor beam 2 is broken
  //if (sensorState2 && !lastState2) {
    //break;
    //Serial.println("Unbroken2");
  //}

  if (!sensorState2 && lastState2) {

    //Serial.println("Broken2");
    
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

    //Serial.print(count);

    delay(500);

  }else if((myarray[0] == 2) && (myarray[1] == 1) && (count >= 1)){

    count--;

    myarray[0] = 0;
    myarray[1] = 0;

    //Serial.println(count);

    delay(500);

  }else if((myarray[0] == 2) && (myarray[1] == 1) && (count == 0)){

    myarray[0] = 0;
    myarray[1] = 0;

    //Serial.println(count);

    delay(500);
  }


  //for servo motor
  if(max_num <= count){

    Servo1.write(90); 
    delay(15);

  }else{

    Servo1.write(0); 
    delay(15);
  }

}
