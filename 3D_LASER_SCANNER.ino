// based on Dan Thompson 2008 http://danthompsonsblog.blogspot.com/
// modified by MagWeb 02.2009

// MURAT KIZILKAYA tarafından değiştirildi HAZİRAN 2015 // MODIFIED BY MK, 06.15
// twitter.com/muratkizilk
// instagram.com/muratkizilk

// TÜRK İŞİ EKİBİ - NTV
// facebook.com/ntv.turkisi
// twitter.com/ntv_turkisi
// instagram.com/ntv_turkisi
// Daha fazla bilgi için ->> www.turkisi.tv
//3d LAZER tarayıcı KODU
//




////////////////////////////////////////////////////////
//Arduino Stepper Motor skecth listening to COM Port
//for use with the EasyDriver 3.1 and DAVID-laserscanner
////////////////////////////////////////////////////////


// based on Dan Thompson 2008 http://danthompsonsblog.blogspot.com/
// modified by MagWeb 02.2009
// Added : Pushbutton to start a single circle or to stop it while it is running
// Added : Endswitch to get zero-value of motion
// Added : LED enlighted while motor moves forward
// Added : Potentiometer to controll motor speed by hand
// Added : second EasyDriver to controll a turntable

// For all the Easydriver v.3 product details http://schmalzhaus.com/EasyDriver/
// For all the DAVID-laserscanner product details http://www.david-laserscanner.com

// Executes a reference motion towards endswitch at character "i"
// Runs scanning circle when you send "g" to the serial port,
// Stops scanning circle when you send "s" to the serial port,
// Switches on light when you send "l" to the serial port,
// Switches off light when you send "d" to the serial port,
// Turns a turntable at 45¡ if you send "t" to the serial port (standard 1.8¡/step),

//Use this code at your own risk and have fun




// Pins
int val = 0;           // stores the state of the BUTTON pin
int LED = 11;           // names pin 9 as LED (placeholder for the laser)
int BUTTON = 7;        // names pin 7 as BUTTON
int END = 8;           // names pin 8 as the endswitch
int LIGHT = 2;         // names pin 2 as the lightswitch
int potpin = 2;        // names pin 2 as "potpin"
int dirpin = 3;        // names pin 3 as "dirpin" -Stepperdirection laser
int steppin = 4;      // names pin 12 as "steppin" -Steppersteps laser
int ttdir = 5;          // names pin 4 as "dirpin" -Stepperdirection turntable
int ttstep = 6;        // names pin 5 as "dirpin" -Steppersteps turntable
//variables
int temp = 0;          // Stores the speed value. The smaller the faster
long range = 300;     // Stores how many steps will be made in total -Stepper laser
int ttrange = 400;      // Stores how many steps will be made in total -Stepper Turntable
int incomingByte = 0;	// for incoming serial data
int old_val = 0;
int state = 0;



void setup() {
  Serial.begin(9600);    // opens serial communication

  pinMode(dirpin, OUTPUT);  // Sets pins to in or out modes
  pinMode(steppin, OUTPUT); //
  pinMode(LED, OUTPUT);     //
  pinMode(BUTTON, INPUT);   //
  pinMode(LIGHT, OUTPUT);   //
  pinMode(END, INPUT);      //
  pinMode(ttdir, OUTPUT);   //
  pinMode(ttstep, OUTPUT);  //
  }


void loop() {
  val=digitalRead(BUTTON);   // read Button input value and store it
  if ((val == HIGH) && (old_val == LOW)) {
    state = 1 - state;
    delay(10);
  }
  
  old_val = val;
  
  
   if (Serial.available() > 0) {
		// read the incoming byte:
		incomingByte = Serial.read();

		// say what you got:
		Serial.print("I received: ");
		Serial.println(incomingByte);
                
	}
  delay(100);
  
  
  
  if (incomingByte == 108) {        // switches on light when "l" is sent by DAVID
    digitalWrite(LED, HIGH);
    incomingByte = 0;
  }
  
  if(incomingByte == 100) {        //switches off light when "d" is sent by DAVID
    digitalWrite(LED, LOW);
    incomingByte = 0;
  }
 
 
  if (incomingByte == 112) {        // switches on light when "l" is sent by DAVID
    digitalWrite(LIGHT, HIGH);
    incomingByte = 0;
  }
  
  if(incomingByte == 113) {        //switches off light when "d" is sent by DAVID
    digitalWrite(LIGHT, LOW);
    incomingByte = 0;
  } 
 
 
  
if (incomingByte == 116) {             // when "t" was sent by DAVID>> turntable travel
  digitalWrite(ttdir, LOW);              // Changes direction
  int i;
      delay(100);

     //////////Serial.println("in motion");           // sends "in motion" to PC
      for (i = 0; i<ttrange+1; i++) {        // pulse
        digitalWrite(ttstep, HIGH);          //   :
        delayMicroseconds(1300);              //   :
        digitalWrite(ttstep, LOW);           //   :
        delayMicroseconds(1300);              // pulse
        
        if (i == ttrange) {                  //stop
          break;
        }
        incomingByte = 0;
      }
  }    
    
    
    
  
  
  if (incomingByte == 105) {             // when "i" was sent by DAVID>> initial travel
  digitalWrite(dirpin, LOW);              // Changes direction
  int i;
      delay(100);

     //////////Serial.println("INITIAL");         // sends "INITIAL" to PC
      for (i = 0; i<range; i++) {        // pulse
        digitalWrite(steppin, HIGH);     //   :
        delayMicroseconds(100);          //   :
        digitalWrite(steppin, LOW);      //   :
        delayMicroseconds(100);          // pulse
        int end = digitalRead (END);
        if (end == HIGH) {               // endswitch
          break;
        }
      }
  }
  
  if ( state == 1 || incomingByte == 103) {                    // If button is pressed
    int i;                              // sets counter of pulses
    temp = 50+analogRead(potpin);     // reads the poti-value and sets speed
    digitalWrite(dirpin, HIGH);         // should be clockwise motormovement
    delay(50);
   //////////Serial.println("SCANNING");               // sends "SCANNING" to PC

    for (i = 0; i<range; i++) {         // pulse
      digitalWrite(steppin, HIGH);      //   :
      delay(temp);          //   :
      digitalWrite(steppin, LOW);       //   :
      delay(temp);          // pulse
      //if (i > 100)  {                   // Waits 100 pulses till LED switches on
        digitalWrite(LED, HIGH);
      //}
      temp = 50+analogRead(potpin);    // reads again the poti-value and sets speed
      incomingByte = Serial.read();
      val = digitalRead (BUTTON);
       if (incomingByte == 115 || i == range-1 || val == HIGH) { // when "s" was sent by DAVID or max range>> stop and backtravel
          digitalWrite(dirpin, LOW);           // Changes direction
          digitalWrite(LED, LOW);              // switch off LED
          
          delay(100);

          //////////Serial.println("BACK");              // sends "BACK" to PC
          for (i = 0; i<range; i++) {      // pulse
          digitalWrite(steppin, HIGH);     //   :
          delay(20);          //   :
          digitalWrite(steppin, LOW);      //   :
          delay(20);          // pulse
          int end = digitalRead (END);  // reads the endswitch

          if (end == HIGH) {            // stop if endswitch reached
            digitalWrite(LED, LOW);       // switch off LED
            i = range;
            incomingByte = 0;
            break;
            }       
          }          
        }        
     }       
delay(100);
  }
val = LOW;
incomingByte = 0;
state = 0;
}


