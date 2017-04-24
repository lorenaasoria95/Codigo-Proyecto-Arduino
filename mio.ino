#include <LiquidCrystal.h>
#include "meArm.h"
#include <Servo.h>

//pines para el LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//pines para el meArm
int basePin = 6;
int elbowPin = 7;
int shoulderPin = 8;
int gripperPin = 9;

//pines para el sensor de color
//s0 & s1 escalado de frecuencia en la salida
//s0 = yellow
//s1 = green
//s0(yellow)  s1(green)
//L           L         = power down no output
//L           H         = 2%
//H           L         = 20%
//H           H         = 100%
//
//EO = set to low - low to enable fo setting above

int s2 = 1; //Sensor color pin s2 to Arduino pin 7
int s3 = 0; //Sensor color pin s3 to Arduino pin 8
int outPin = 14; //Sensor color pin OUT to Arduino pin 4
int s0=10;  //18
int s1=13;  //19

meArm arm(
      180,0, -pi/2, pi/2,    //Base     (-90 to  90 degrees)
      125,37, pi/4, 3*pi/4,   //Shoulder ( 45 to 135 degrees)
      89,44, 0, -pi/4,      //Elbow    (  0 to -45 degrees)
      40,140, pi/2, 0);        //Pinza open to close  ( 90 to   0 degrees)

boolean DEBUG = true;

void setup() {
  arm.begin(basePin, shoulderPin, elbowPin, gripperPin);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT);   
  
  lcd.begin(16, 2);   //definimos columnas y filas del lcd
  lcd.print("Bienvenidos!");
  delay(5000);
  lcd.clear();
  Serial.begin(9600); //turn on serial port

  //move the arm to the home position
  //arm.gotoPoint(-110,80,60);
  //delay(1000);
  //arm.closeGripper();
  //delay(100);
}

void loop() {

  //***** grab a color token *****
  arm.gotoPoint(70,160,0);
  arm.openGripper();
  
  //localización fichas
  arm.gotoPoint(70,165,-15); //coge una ficha
  delay(1000);
  //coger objeto
  arm.closeGripper();
  delay(100);
  arm.gotoPoint(70,165,40);

  //***** lleva al sensor de color
  arm.gotoPoint(-119,55,30);  //-169,51,45
  delay(1000); //ESTE DELAY ES PROBLEMATICO

  //identifica al color
  char color = get_color();

  switch(color) {
    case 'r':   //red
      arm.gotoPoint(-20,170,50);
      delay(500); //pause
      arm.openGripper();
      //arm.gotoPoint(-110,170,50);
      break;
	  
    case 'y':
      arm.gotoPoint(290,170,50);
      delay(500); //pause
      arm.openGripper();
      //arm.gotoPoint(-110,170,50);
      break;
	  
    case 'u':   //color no identificado
      arm.gotoPoint(0,50,50);
      delay(500); //pause
      arm.openGripper();
      break;
  }
  
  delay(500);  
}

char get_color() {

  int R;
  int G;
  int B;

  char color;
   
  //read red component
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  R = pulseIn(outPin, LOW);

 //read green component
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  G = pulseIn(outPin, LOW);
  
 //let's read blue component
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  
  B = pulseIn(outPin, LOW);

  //*********** calibration required ***************
  //Color sensor is very sensitive to ambient light. You must calibrate following range of values for green, red, and yellow chips.
  //The following values are obtained when a color token is placed at appprox 1 inch from the sensor 
  //Color
  //RGB values for tokens are:
  //red:  30<R<60, 70<G<120, 50<B<90
  //yellow: 10<R<25, 10<G<40, 20<B<50
  //green: 40<R<70, 20<G<50, 20<B<50 daytime calibration
  
  if((R>120) && (R<171) && (G>120) && (G<171) && (B>120) && (B<171))    // red PARA MOVIL EDU (R>85) && (R<130) && (G>90) && (G<130) && (B>100) && (B<130)
    {
      if (DEBUG) {
        Serial.println("Color is RED");
        Serial.print("R = ");
        Serial.print(R);
        Serial.print("  G = ");
        Serial.print(G);
        Serial.print("  B = ");
        Serial.println(B);
        //lcd.autoscroll();
        lcd.print("Color:");
        lcd.setCursor(0,1);
        lcd.print("Rojo");
        delay(2000);
        lcd.clear();
      }
      color = 'r';
    }
  else if((R>10) && (R<100) && (G>10) && (G<100) && (B>10) && (B<100))  // yellow PARA MOVIL EDU (R>10) && (R<35) && (G>10) && (G<40) && (B>15) && (B<60
    {
      if (DEBUG) {
        Serial.println("Color is YELLOW");
        Serial.print("R = ");
        Serial.print(R);
        Serial.print("  G = ");
        Serial.print(G);
        Serial.print("  B = ");
        Serial.println(B);
        //lcd.autoscroll();
        lcd.print("Color:");
        lcd.setCursor(0,1);
        lcd.print("Amarillo");
        delay(2000);
        lcd.clear();
      }
      color = 'y';
    }
  else 
    {  
      if (DEBUG) {
        Serial.println("El color es desconocido");
        Serial.print("R = ");
        Serial.print(R);
        Serial.print("  G = ");
        Serial.print(G);
        Serial.print("  B = ");
        Serial.println(B);
        //lcd.autoscroll();
        lcd.print("Color:");   
        lcd.setCursor(0,1);
        lcd.print("N/A");
        delay(2000);
        lcd.clear();
      }
      color = 'u'; //unidentified
    }

  return color;
  
}

