#include <Wire.h> // 아두이노와 LCD간의 통신을 위한 라이브러리 사용
#include <LiquidCrystal_I2C.h> // LCD 모니터 라이브러리 사용
LiquidCrystal_I2C lcd(0x3F, 16, 2); // 사용할 LCD를 정의

//#define trig 8
//#define echo 9

int resi = A0; // 가변저항 핀 번호
int pmb = 13; // 분/시간을 바꾸는 버튼 핀 번호
int okb = 12; // 확인하는 버튼 핀 번호
int stat = 0; // 현재 시간 설정 상태를 나타내는 변수
int ahour = 0; // 알람 시간 변수
int amin = 0; // 알람 분 변수
int fstat = 0; // 시간/분 설정 상태를 나타내는 변수
// long duration, distance; 

void setup() { // 반복 전 미리 설정하는 함수
  pinMode(pmb, OUTPUT); // 해당 핀을 출력 모드로 사용
  pinMode(okb, OUTPUT); // 해당 핀을 출력 모드로 사용
//  pinMode(trig, OUTPUT); // 해당 핀을 출력 모드로 사용
//  pinMode(echo, OUTPUT); // 해당 핀을 출력 모드로 사용
  lcd.begin(); // lcd 작동 시작
  lcd.backlight(); // lcd 빛 작동
}



void loop() {
//  digitalWrite(trig, LOW);
//  digitalWrite(echo, LOW);
//  delay(2);
//  digitalWrite(trig, HIGH);
//  delay(10);
//  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 170 / 1000;
  
  if (digitalRead(okb)) { // 상태 변경 버튼이 눌린 경우
    stat = 1-stat; // 상태를 변경
  }
  if (digitalRead(pmb)) { // 시간/분 변경 버튼이 눌린 경우
    fstat = 1-fstat; // 상태를 변경
  }
  if (stat) { // 현재 상태가 알람 설정 상태인 경우
    if (fstat) { // 현재 상태가 시간 변경 상태인 경우
      ahour = map(analogRead(resi), 0, 1023, 0, 24); // 알람 시간을 가변저항의 변환 값으로 설정
    } else { // 분 변경 상태인 경우
      amin = map(analogRead(resi), 0, 1023, 0, 60); // 알람 분을 가변저항의 변환 값으로 설정
    }
  }
  lcd.clear(); // LCD 모니터 초기화
  lcd.setCursor(0,0); // lcd 모니터의 커서를 0,0(왼똑 위)로 설정
  if (ahour < 10) { // 알람 시간이 10보다 작은 경우
    lcd.setCursor(1,0); // 빈칸 설정
  }
  lcd.print(ahour); // 알람 시간을 lcd에 출력
  lcd.print(":"); // 시간 구분 특수기호 '콜론'을 출력
  if (amin < 10) { // 알람 분이 10보다 작은 경우
    lcd.print(" "); // 빈칸 설정
  }
  lcd.print(amin); // 알람 분 출력
//  lcd.print("/");
//  lcd.print(distance);
  lcd.setCursor(0,1); // lcd 모니터의 커서를 0,1(왼쪽 아래)로 설정
  if (stat) { // 현재 상태가 알람 변경 상태인 경우
    lcd.print("Fix Mode "); // Fix Mode 출력
    if (fstat) { // 시간 변경 상태인 경우
      lcd.print("Hour"); // Hour 출력
    } else { // 분 변경 상태인 경우
      lcd.print("Min"); // Min 출력
    }
  } else { // 알람 상태인 경우
    lcd.print("Alarm Mode"); // Alarm Mode 출력
  }
}
