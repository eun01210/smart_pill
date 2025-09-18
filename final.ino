#include <ThreeWire.h> // IIC 통신 (RTC 모듈과 통신하는 라이브러리)
#include <RtcDS1302.h> // RTC 모듈 라이브러리
#include <Servo.h> // Servo 라이브러리
#include <Wire.h> // 아두이노와 LCD간의 통신을 위한 라이브러리 사용
#include <LiquidCrystal_I2C.h> // LCD 모니터 라이브러리 사용

LiquidCrystal_I2C lcd(0x3F, 16, 2); // 사용할 LCD를 정의

#define trig 5 // 트리거 핀 선언
#define echo 6 // 에코 핀 선언

Servo myservo; // myservo 라는 Servo 모듈을 생성

int hour = 0; // 시간 단위 알람
int minu = 0; // 분 단위 알람
int nhour = -1; // 현재 시간 변수
int nminu = -1; // 현재 분 변수
int nseco = -1; // 현재 초 변수
int buzzer = 10; // 부저 핀 번호
int resi = A0; // 가변저항 핀 번호
int pmb = 13; // 분/시간을 바꾸는 버튼 핀 번호
int okb = 12; // 확인하는 버튼 핀 번호
int stat = 0; // 현재 시간 설정 상태를 나타내는 변수
int ahour = 0; // 알람 시간 변수
int amin = 0; // 알람 분 변수
int fstat = 0; // 시간/분 설정 상태를 나타내는 변수
int pmbw = 0; // 분/시간 버튼 대기 시간
int okbw = 0; // 확인 버튼 대기 시간

//8핀이 DAT
//9핀이 CLK
//7핀이 CE
ThreeWire myWire(8,9,7); // 통신을 위한 선이 연결되어 있음.
// 8번:DAT, 9번:CLK, 7번:CE (IO, SCLK, CE)

RtcDS1302<ThreeWire> Rtc(myWire); // RTC 모듈 생성

void setup () // 반복 전 미리 설정해 두는 함수
{
    pinMode(buzzer, OUTPUT); // buzzer가 꽂힌 핀을 출력용으로 사용
    pinMode(trig, OUTPUT); // 트리거 핀 출력으로 선언
    pinMode(echo, INPUT); // 에코 핀 입력으로 선언
    pinMode(pmb, OUTPUT); // 해당 핀을 출력 모드로 사용
    pinMode(okb, OUTPUT); // 해당 핀을 출력 모드로 사용
    lcd.begin(); // lcd 작동 시작
    lcd.backlight(); // lcd 빛 작동
    myservo.attach(11); // myservo가 12번 핀에 꽂혀 있음
    Rtc.Begin(); // RTC 모듈 작동
    
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); //RTCDateTime 클래스 생성(컴파일된 시간으로 설정)

    if (Rtc.GetIsWriteProtected()) //RTC모듈에 쓰기 금지 모드인지 확인
    {
        Rtc.SetIsWriteProtected(false); //쓰기 금지 모드이면 해제
    }
    
    if (!Rtc.GetIsRunning()) //RTC 모듈이 동작 중이 아닌 경우
    {
        Rtc.SetIsRunning(true); // RTC 모듈 작동 시작
    }
 
    RtcDateTime now = Rtc.GetDateTime(); // RTC 모듈의 현재 시간 얻기

    //RTC 모듈하고 위에 만든 RTC클래스(컴파일된 시간을 가지고 있는)와 비교해서
    //RTC 시간이 더 늦다면.. 
    if (now < compiled) // RTC모듈의 시간보다 컴파일된 시간이 빠른 경우
    {
        Rtc.SetDateTime(compiled); // RTC모듈을 컴파일된 시간으로 갱신
    }
}

void loop () // 무한 반복 함수
{
    RtcDateTime now = Rtc.GetDateTime(); //RTC 모듈의 현재 시간 얻기

    long duration, distance; // 거리 측정을 위한 변수 선언
    // 트리거 핀으로 10us 동안 펄스 출력
    digitalWrite(trig, LOW); // Trig 핀 Low
    digitalWrite(trig, HIGH); // Trig 핀 High
    digitalWrite(trig, LOW); // Trig 핀 Low

    /*
        에코핀에서 받은 펄스 값을 pulseIn함수를 호출하여
        펄스가 입력될 때까지의 시간을 us단위로 duration에 저장
        pulseln() 함수는 핀에서 펄스(HIGH or LOW)를 읽어서 마이크로초 단위로 반환
    */
 
    duration = pulseIn(echo, HIGH); // 음파 왕복 시간 설정
 
   /*
        음파의 속도는 초당 340m, 왕복하였으니 나누기 2를하면 170m이고,
        mm단위로 바꾸면 170,000mm.
        duration에 저장된 값은 us단위이므로 1,000,000으로 나누어 주고,
        정리해서 distance에 저장 
   */
 
    distance = duration * 170 / 1000; // 거리는 왕복 시간을 적당한 수로 나눠서 mm로 환산
    
    if (nseco != now.Second()) { // 현재 초가 실제 초와 다른 경우
      nhour = now.Hour(); // 현재 시간을 실제 시간으로 설정
      nminu = now.Minute(); // 현재 분을 실제 분으로 설정
      nseco = now.Second(); // 현재 초를 실제 초로 설정
      if (pmbw > 0) { // 분/시간 버튼이 대기 중인 경우
        pmbw--; // 대기 시간 1 감소
      }
      if (okbw > 0) { // 확인 버튼이 대기 중인 경우
        okbw--; // 대기 시간 1 감소
      }
    }

    if (ahour == nhour && amin == nminu && stat == 0) { // 현재 시간과 분이 일치한 경우
      myservo.write(0); // servo의 각도 설정
    } else { // 알람에 맞춘 시간이 아닌 경우
      myservo.write(180); // servo의 각도 설정
    }

    if (distance <= 200) { // 초음파 센서와의 거리가 200 이하인 경우 (약 봉투가 있는 경우)
      digitalWrite(buzzer, HIGH); // 부저 작동
    } else { // 아닌 경우
      digitalWrite(buzzer, LOW); // 부저 종료
    }

    if (digitalRead(okb) && okbw == 0) { // 상태 변경 버튼이 눌린 경우
      stat = 1-stat; // 상태를 변경
      okbw = 2; // 확인 버튼 대기 시간 설정
    }
    if (digitalRead(pmb) && pmbw == 0) { // 시간/분 변경 버튼이 눌린 경우
      fstat = 1-fstat; // 상태를 변경
      pmbw = 2; // 시간/분 버튼 대기 시간 설정
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
