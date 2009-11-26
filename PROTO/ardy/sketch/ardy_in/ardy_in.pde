int s, t;

void setup() {
  pinMode(2, INPUT);
  Serial.begin(9600);
}

void loop() {
  s = digitalRead(2);
  if (s != t) {
    if (digitalRead(2) == HIGH) {
      Serial.println("Pressed");
    } 
  }
  t = s;
}
