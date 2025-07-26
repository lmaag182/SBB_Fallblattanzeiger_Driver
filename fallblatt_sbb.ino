#include <Bounce2.h>

// Initializing pins
const int pwr_a = 3;
const int pwr_b = 9;
const int dir_a = 2;
const int dir_b = 8;

// Step delay, is used as delay between each half-step, in the motor driver. 
// This delay is measured in microseconds.
int stepDelay = 5;//5 miliseconds

// Instantiate a Bounce object
Bounce debouncerPin1 = Bounce();
Bounce debouncerPin2 = Bounce();
Bounce debouncerPin3 = Bounce();
Bounce debouncerPin4 = Bounce();
Bounce debouncerPin5 = Bounce();
Bounce debouncerPin6 = Bounce();
Bounce gap = Bounce();


//is the motor currently running
int motor = 0;

int targetScreen = 0;

int gapFellCounter = 0; 


//the sequence of the labels matching the pattern of the encoder
//so index 0 of the array is matches the representation of "0." on the "hour" module
//index 1 matches "1." and so forth
byte labelSequence [] = {
  0b110000,0b010001,0b010000,0b100011,0b100010,
  0b110011,0b000101,0b110111,0b111110,0b011111,
  0b011110,0b111101,0b111100,0b011101,0b011100,
  0b101111,0b101110,0b001111,0b001110,0b101101,
  0b101100,0b001101,0b001100,0b111011,0b111010,
  0b011011,0b011010,0b111001,0b111000,0b011001,
  0b011000,0b101011,0b101010,0b001011,0b010100,
  0b100100,0b110010,0b010011,0b010010,0b110001
  };


void setup(){
  Serial.begin(9600);
  // Initialize the pins, in the correct type of mode.
  pinMode(pwr_a,OUTPUT);
  pinMode(pwr_b,OUTPUT);
  pinMode(dir_a,OUTPUT);
  pinMode(dir_b,OUTPUT);

  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP); 
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);

  debouncerPin1.attach(4);
  debouncerPin1.interval(5); // interval in ms
  debouncerPin1.init(digitalRead(4));

  debouncerPin2.attach(5);
  debouncerPin2.interval(5); // interval in ms
  debouncerPin2.init(digitalRead(5));

  debouncerPin3.attach(6);
  debouncerPin3.interval(5); // interval in ms
  debouncerPin3.init(digitalRead(6));

  debouncerPin4.attach(7);
  debouncerPin4.interval(5); // interval in ms
  debouncerPin4.init(digitalRead(7));

  debouncerPin5.attach(10);
  debouncerPin5.interval(5); // interval in ms
  debouncerPin5.init(digitalRead(10));

  debouncerPin6.attach(11);
  debouncerPin6.interval(5); // interval in ms
  debouncerPin6.init(digitalRead(11));
  
  gap.attach(11);
  gap.interval(0); // interval in ms
  
  gap.init(gapValue());
  
  motor = 0;

}

void loop(){


  // Update the Bounce instance :
  debouncerPin1.update(digitalRead(4));
  debouncerPin2.update(digitalRead(5));
  debouncerPin3.update(digitalRead(6));
  debouncerPin4.update(digitalRead(7));
  debouncerPin5.update(digitalRead(10));
  debouncerPin6.update(digitalRead(11));
  gap.update(gapValue());

  
  // Get the updated value :
  //int value = debouncerPin1.read();
  //Serial.println(gapValue());

  if (gap.fell()){
    //delay(50);
    byte c = readCurrentState();
    if (c == labelSequence[targetScreen]){
      Serial.println("---end of cycel-----");
          motor = 0;
          TurnOfMotors();
    }
    
    //Serial.println("gap fell");
    printCurrentState();
    Serial.print(":");
    Serial.print(gapFellCounter,BIN);
    Serial.print(":");
    Serial.println(readCurrentState(),BIN);
    gapFellCounter++;
  }

//  if (Serial.available() > 0) {
//    // get incoming byte:
//    byte inByte = Serial.read();
//    Serial.print(inByte);
//  }

   while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    int red = Serial.parseInt();
//    // do it again:
//    int green = Serial.parseInt();
//    // do it again:
//    int blue = Serial.parseInt();

    // look for the newline. That's the end of your
    // sentence:
    if (Serial.read() == '\n') {
      // constrain the values to 0 - 255 and invert
      // if you're using a common-cathode LED, just use "constrain(color, 0, 255);"
//      red = 255 - constrain(red, 0, 255);
//      green = 255 - constrain(green, 0, 255);
//      blue = 255 - constrain(blue, 0, 255);

      //if (targetScreen != red){
        motor = 1;
        targetScreen = red;
      //}
      // print the three numbers in one string as hexadecimal:
      Serial.print(red);
//      Serial.print(green, HEX);
//      Serial.println(blue, HEX);
    }
  }
//  if (debouncerPin1.fell()){
//     //Serial.println("max reached");
//  }
//  if (gapFellCounter > 39){
//    //Serial.println("max reached");
//  }

  if (motor == 1){
    Step4BWD(1);
  }
  
  // Test loop, should make your stepper go 40 steps forewards and backwards, with 1 second delay.

  //Step4FWD(10);
  //delay(1000);
  //Step4BWD(6);
  //delay(1000);
  //int sensorValue = digitalRead(0);
  //Serial.println(sensorValue);
}


// drives the synchronous motor 1 cycle forward

void Step4FWD(int NumberOfTimes){
  
  // The function will run for the amount of times called in the method.
  // This is accomplished by a while loop, where it will subtract 1 from the amount, after every run.
  
  //while(NumberOfTimes!=0){
    // Starting position (if repeated, ful step (4))
    // EXPLINATION: in this case, both our power are high.
    // Therefore both coils are activated, with their standard polarities for their magnetic fields.
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,HIGH);
    digitalWrite(dir_b,HIGH);
 
 
    delay(stepDelay);
 
    //Full step (1)
    // EXPLINATION: In this case, the b-coil is activated as in previous cases.
    // But the a-coil now has it's direction turned on. So now it's active, but with the reversered polarity.
    // By continuing this pattern (for reference: http://www.8051projects.net/stepper-motor-interfacing/full-step.gif) , you'll get the axis to turn.
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,HIGH);
    digitalWrite(dir_b,LOW);

 
    delay(stepDelay);
 
    // Full step (2)
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,LOW);
    digitalWrite(dir_b,LOW);

 
    delay(stepDelay);
 
    // Full step (3)
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,LOW);
    digitalWrite(dir_b,HIGH);

    delay(stepDelay);


    //NumberOfTimes--; 
  //}
 
 TurnOfMotors();
 }

// drives the synchronous motor 1 cycle backward
// most of motorshields can be used to drive the motor
void Step4BWD(int NumberOfTimes){

  //while(NumberOfTimes!=0){
 
    // Starting position (if repeated, ful step (4))
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,LOW);
    digitalWrite(dir_b,LOW);
 
    delay(stepDelay);
 
    // Full step (1)
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,HIGH);
    digitalWrite(dir_b,LOW);

    delay(stepDelay);
 
    // Full step (2)
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,HIGH);
    digitalWrite(dir_b,HIGH);
 
    delay(stepDelay);
 
    // Full step (3)
    digitalWrite(pwr_a,HIGH);
    digitalWrite(pwr_b,HIGH);
    digitalWrite(dir_a,LOW);
    digitalWrite(dir_b,HIGH);

    delay(stepDelay);

 
    //NumberOfTimes--;
  //}
 
 //TurnOfMotors();
}

// This method simply just turn of the motors, called when ever we don't need the motors anymore.
// In this way, we won't fray the circuit or coils.
void TurnOfMotors(){
  digitalWrite(pwr_a,LOW);
  digitalWrite(pwr_b,LOW);
  digitalWrite(dir_a,LOW);
  digitalWrite(dir_b,LOW);
}


int gapValue(){
   if (
    (debouncerPin1.read() == 1) &&
    (debouncerPin2.read() == 1) &&
    (debouncerPin3.read() == 1) &&
    (debouncerPin4.read() == 1) &&
    (debouncerPin5.read() == 1) &&
    (debouncerPin6.read() == 1)
  )
  {
    return 1;
  }else{
    return 0;
  }
}
void printCurrentState(){
  
    Serial.print(digitalRead(4));
    Serial.print(digitalRead(5));
    Serial.print(digitalRead(6));
    Serial.print(digitalRead(7));
    Serial.print(digitalRead(10));
    Serial.print(digitalRead(11));
}
byte readCurrentState(){
  byte s = 0b000000;
  if (debouncerPin1.read()==1){
    s  =  s | 1 << 5;
  };
  if (debouncerPin2.read()==1){
    s  |= 1 << 4;
  };
  if (debouncerPin3.read()==1){
    s  |= 1 << 3;
  };
  if (debouncerPin4.read()==1){
    s  |= 1 << 2;
  };
  if (debouncerPin5.read()==1){
    s  |= 1 << 1;
  };
  if (debouncerPin6.read()==1){
    s  |= 1 << 0;
  };

  return s;
}

