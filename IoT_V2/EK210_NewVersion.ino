#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Servo.h>
#include <Keypad.h>
#include <stdlib.h>
#include <string.h>

const byte ROWS = 4; 
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '10'},
  {'4', '5', '6', '11'},
  {'7', '8', '9', '12'},
  {'c', '0', '14', 'r'}
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

int mynum = -1;
int count_char = 10;

void loop(){
  // read the state of the pushbutton value:
  sensorState = digitalRead(SENSORPIN);

  sensorState2 = digitalRead(SENSORPIN2);

  // check command change from phone
  String command = " ";
  command = Serial.readString();
  if(command == "+"){
    max_num++;
    mem_number[0] = (char) (max_num%10);
    mem_number[1] = (char) (((max_num-max_num%10)/10)%10);
    mem_number[2] = (char) (((max_num - 10*(((max_num-max_num%10)/10)%10) - max_num%10)/100)%10);
  }
  if(command == "-"){
    max_num--;
    mem_number[0] = (char) (max_num%10);
    mem_number[1] = (char) (((max_num-max_num%10)/10)%10);
    mem_number[2] = (char) (((max_num - 10*(((max_num-max_num%10)/10)%10) - max_num%10)/100)%10);
  }

  char current_input = customKeypad.getKey();

  if(current_input!=0){
    if(current_input == 'r'){
      mynum = -1;
      count_char = 10;
      max_num = 5;
      memset(mem_number, 0, 255);;
    }else {
      strncat(mem_number,&current_input,1);
      max_num = (int) mem_number;
      count_char += 1;
    }
  }

  if (mynum >= 0){
  
    max_num = mynum;
  }

  // task for LCD Screen

  lcd.setCursor(0, 0);
  lcd.print("Maximum : ");

  lcd.setCursor(10, 0);
  lcd.print("    pp");

  lcd.setCursor(10, 0);
  if(mem_number == ""){
    lcd.print(max_num);
    Serial.println(max_num);
    Serial.println(" ");
    Serial.println(count);
  }else{
    lcd.print(mem_number);
    Serial.println(mem_number);
    Serial.println(" ");
    Serial.println(count);
  }

  lcd.setCursor(0, 1);
  lcd.print("Current : ");

  lcd.setCursor(10, 1);
  lcd.print("    pp");

  lcd.setCursor(10, 1);
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

    delay(1500);

  }else if((myarray[0] == 2) && (myarray[1] == 1) && (count == 0)){

    myarray[0] = 0;
    myarray[1] = 0;

    //Serial.println(count);

    delay(1500);
  }


  //for servo motor
  if(max_num <= count){

    Servo1.write(90); 
    delay(150);

  }else{

    Servo1.write(0); 
    delay(150);
  }

}