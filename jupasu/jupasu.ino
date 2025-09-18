int buzzer = 9;
int button = 13;
int pbut = 11;
int mbut = 10;
int trans = A0;
int nowhz = 0;
int tra = 0;
int temp = 1;

String readSerial() {   
   String str = "";   
   char ch;   

    while( Serial.available() > 0 ) {   
      ch = Serial.read();   
      str.concat(ch);   
      delay(10);  
    }   
    return str;     
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
  pinMode(pbut, INPUT);
  pinMode(mbut, INPUT);
  pinMode(trans, INPUT);
  Serial.begin(57600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(button) == HIGH) {
    tone(buzzer, nowhz, 100);
  }
  tra = map(analogRead(trans), 0, 1023, 0, 5);
  String str;
  str = readSerial();
  if (str != "") {
    Serial.print("현재 주파수 : ");
    Serial.println(nowhz);
    Serial.print("현재 저항 값 : ");
    Serial.println(tra);
    Serial.println("");
  }
  if (digitalRead(pbut) == HIGH) {
    temp = 1;
    if (tra > 1) {
      for(int i=1;i<tra;i++) {
        temp = temp*10;
      }
    }
    nowhz = nowhz + temp;
    delay(300);
  }
  if (digitalRead(mbut) == HIGH) {
    temp = 1;
    if (tra > 1) {
      for(int i=1;i<tra;i++) {
        temp = temp*10;
      }
    }
    nowhz = nowhz - temp;
    delay(300);
  }
}
