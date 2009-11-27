/*
    Ardy OUT - An example of getting signals out from 
               the Edje file.

    
    Setup:   (----|----(10)    GREEN
             (4pin|------(11)  BLUE
             (LED |--------(-ve)
             (----|----(9)     RED


    Edje Code:  It will send DEC values for single
                characters over the serial line.
                
*/

int val, l; // Boring little buffers

const int RED = 9;    //Red pin with PWM
const int GREEN = 10; //Green pin w/ PWM
const int BLUE = 11;  //Blue pin w/PWM

void blank() {            // Function to clear the LED
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

void light(int l) {
  blank();                //Clear..
  digitalWrite(l, HIGH);  //And light up!
}

void setup() {
  pinMode(RED, OUTPUT);    //Turn pins into OUTPUTers
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(9600);      //Setup a serial connection
}

void loop() {
  if (Serial.available()) { //We got serial?
    val = Serial.read();  //Read and print...
    Serial.println(val, DEC); //Dec values for...
    if (val == 48) { //0
      blank(); 
    }
    if (val == 49) { //1
      light(RED); 
    }
    if (val == 50) { //2
      light(GREEN); 
    }
    if (val == 51) {  //3 
      light(BLUE); 
    }
  }
}




