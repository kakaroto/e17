int val;
int l = 0;

void blank() {
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

void light(int l) {
  blank();
  digitalWrite(l, HIGH);
}

void setup() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  Serial.flush();
}

void loop() {
  if (Serial.available()) {
    val = Serial.read();
    Serial.println(val, DEC); //Dec values for...
    if (val == 48) { //0
      blank(); 
    }
    if (val == 49) { //1
      light(10); 
    }
    if (val == 50) { //2
      light(9); 
    }
    if (val == 51) {  //3 
      light(11); 
    }
  }
}



