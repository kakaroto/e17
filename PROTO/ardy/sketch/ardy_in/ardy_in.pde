/*
    Ardy IN - An example of getting signals into 
              the Edje file.

    
    Setup:     ---| Button |---(5v+)
           (2) ---|        |---(1k ohm)---(-)


    Edje Code: It will listen from source "arduino"
               for a "P" signal to come down the line.
               Single characters are only supported
               at the moment for reliability.
*/

int s, t; // Counters for bounce

void setup() {
  Serial.begin(9600); //Setup a serial connection
  pinMode(2, INPUT);  //Pin 2 for button reads
}

void loop() {
  s = digitalRead(2);    //Read Button and store it
  if (s != t) {          //Is it the same as last read?
    if (s == HIGH) {     //If its pushed...
      Serial.print("P"); //Send a P to Edje!
      delay(20);         //Take a break and chill out.
    }
  }
  t = s;                 //Store last button state
}
