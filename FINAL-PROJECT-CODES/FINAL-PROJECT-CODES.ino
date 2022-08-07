/*
  Ambient light
  low = 0
  high = 471
*/
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(32, 16, 2);
Servo window;

// Ambient light pin
int ambientPin = A3;
int ambientVal;

// IR pin
int IRpin = 11;
IRrecv IR(IRpin);

// Temperature sensor pin
int tmpPin = A0;
int tmpVal;
int tmpValC;

// PIR pin
int PIRpin = 12;
int pirVal;

// Relay pin
int relay_fan1 = 3;
int relay_fan2 = 4;
int relay_fan3 = 7;
int relay_bulb = 2;

// RGB pin
int redPin = 5;
int greenPin = 9;
int bluePin = 6;

// millis()
unsigned long prevTime;
int delayInterval = 50;
int push;

void setup()
{
  Serial.begin(9600);
  // OUTPUT
  pinMode(relay_fan1, OUTPUT);
  pinMode(relay_fan2, OUTPUT);
  pinMode(relay_fan3, OUTPUT);
  pinMode(relay_bulb, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // INPUT
  pinMode(ambientPin, INPUT);
  pinMode(PIRpin, INPUT);
  pinMode(tmpPin, INPUT);
  pinMode(IRpin, INPUT);
  IR.enableIRIn();

  window.attach(10);
  window.write(0);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setBacklight(0);
}

void loop()
{
  readValues();
  IR_remoteControl();
  
  if (pirVal == 1)
  {
    lcd.setBacklight(1);
    if (tmpValC >= 24 && tmpValC <= 30)
    {
      window.write(0);
      digitalWrite(relay_fan1, HIGH);
      digitalWrite(relay_fan2, LOW);
      digitalWrite(relay_fan3, LOW);
      
      lcd.setCursor(0,0);
      lcd.print("NORMAL MODE:");
      lcd.setCursor(0,1);
      lcd.print("Temp= 24-30deg");
      lcd.setCursor(14,1);
      lcd.print("  ");
    }
    else if (tmpValC >= 31 && tmpValC <= 36)
    {
      window.write(0);
      digitalWrite(relay_fan1, LOW);
      digitalWrite(relay_fan2, HIGH);
      digitalWrite(relay_fan3, LOW);
      
      lcd.setCursor(0,0);
      lcd.print("FAST MODE:");
      lcd.setCursor(0,1);
      lcd.print("Temp= 31-36deg");
      
    }
    else if (tmpValC >= 37)
    {
      window.write(180);
      digitalWrite(relay_fan1, LOW);
      digitalWrite(relay_fan2, LOW);
      digitalWrite(relay_fan3, HIGH);
      
      lcd.setCursor(0,0);
      lcd.print("VERY FAST MODE:");
      lcd.setCursor(0,1);
      lcd.print("Temp= 37deg ABOVE");
      
    }
    if (ambientVal < 233 || push == 11)
    {
      digitalWrite(relay_bulb, HIGH);
    }
    else if (ambientVal >= 233 || push == 0)
    {
      digitalWrite(relay_bulb, LOW);
    }
    if(push == 1)
    {
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
    }
    else if(push == 2)
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
    }
    if(push == 3)
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 255);
    }
    if(push == 12)
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
    }
  }
  else
  {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
    window.write(0);
    digitalWrite(relay_fan1, LOW);
    digitalWrite(relay_fan2, LOW);
    digitalWrite(relay_fan3, LOW);
    digitalWrite(relay_bulb, LOW);
    lcd.setBacklight(0);
  }
}

void readValues()
{
  ambientVal = analogRead(ambientPin);
  tmpVal = analogRead(tmpPin);
  tmpValC = map(tmpVal, 20, 358, 24, 43);
  Serial.println(tmpValC);
  pirVal = digitalRead(PIRpin);
}

void IR_remoteControl()
{
  while (IR.decode())
  {
    //Serial.println(IR.decodedIRData.decodedRawData, HEX);
    if (millis() - prevTime >= delayInterval)
    {
      if (IR.decodedIRData.decodedRawData == 0xFF00BF00)
      {
        Serial.println("Power");
        push = 10;
      }
     
      if (IR.decodedIRData.decodedRawData == 0xFD02BF00)
      {
        Serial.println("Func/Stop");
        push=12;
      }
     
      if (IR.decodedIRData.decodedRawData == 0xFA05BF00)
      {
        Serial.println("Pause/Play");
        push = 11;
      }
      if (IR.decodedIRData.decodedRawData == 0xF906BF00)
      {
        Serial.println("Fast forward");
      }
      
     
      if (IR.decodedIRData.decodedRawData == 0xF30CBF00)
      {
        Serial.println("0");
        push = 0;
      }
      
      if (IR.decodedIRData.decodedRawData == 0xEF10BF00)
      {
        Serial.println("1");
        push = 1;
      }
      if (IR.decodedIRData.decodedRawData == 0xEE11BF00)
      {
        Serial.println("2");
        push = 2;
      }
      if (IR.decodedIRData.decodedRawData == 0xED12BF00)
      {
        Serial.println("3");
        push = 3;
      }
    }
    IR.resume();
  }
}
