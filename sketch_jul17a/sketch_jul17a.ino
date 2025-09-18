#include <ThreeWire.h> // IIC 통신 (RTC 모듈과 통신하는 라이브러리)
#include <RtcDS1302.h> // RTC 모듈 라이브러리
#include <Servo.h> // Servo 라이브러리

Servo myservo; // myservo 라는 Servo 모듈을 생성

int hour = 0; // 시간 단위 알람
int minu = 0; // 분 단위 알람
int nhour = -1; // 현재 시간 변수
int nminu = -1; // 현재 분 변수
int nseco = -1; // 현재 초 변수
int buzzer = 13; // buzzer가 13번 핀에 연결됨
int led = 11; // led가 11번 핀에 연결됨
int button = 10; // button이 10번 핀에 연결됨
unsigned long t = 1; // led 또는 buzzer가 작동한 시간
float pt = 0; // led 또는 buzzer가 작동한 후, 반응 까지 걸린 시간
int rd = -1; // 신호를 준 후, 랜덤 초 후에 led 또는 buzzer가 작동하게 하게 하기 위한 변수

//8핀이 DAT
//9핀이 CLK
//7핀이 CE
ThreeWire myWire(8,9,7); // 통신을 위한 선이 연결되어 있음.
// 8번:DAT, 9번:CLK, 7번:CE (IO, SCLK, CE)

RtcDS1302<ThreeWire> Rtc(myWire); // RTC 모듈 생성

void setup () // 반복 전 미리 설정해 두는 함수
{
    pinMode(buzzer, OUTPUT); // buzzer가 꽂힌 핀을 출력용으로 사용
    pinMode(led, OUTPUT); // led가 꽂힌 핀을 출력용으로 사용
    pinMode(button, INPUT); // button이 꽂힌 핀을 입력용으로 사용
    myservo.attach(12); // myservo가 12번 핀에 꽂혀 있음
    Serial.begin(57600); // 시리얼 모니터를 57600의 통신 속도로 사용
    Rtc.Begin(); // RTC 모듈 작동
    /*
    //컴파일 시점의 날짜(__DATE__)와 시간(__TIME__)을 시리얼모니터에 표시
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__)*/

    
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); //RTCDateTime 클래스 생성(컴파일된 시간으로 설정)
//    printDateTime(compiled);
//    Serial.println();

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
    /*
    //RTC 모듈이 컴파일한 시간보다 빠르다면
    else if (now > compiled) 
    {
        //RTC가 캄파일 시간보다 더 새것이다(그치 이래야지!!!)
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    //RTC와 컴파일한 시간이 같다면.
    else if (now == compiled) 
    {
        //RTC와 컴파일 시간이 같다. (그..그럴이가... 뭐 그래도 오케!)
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }*/
}

String readSerial() // 시리얼 모니터에 입력한 값을 불러오는 함수
{   
   String str = ""; // 값을 저장할 텍스트 형식 변수 선언
   char ch; // 모든 값을 확인하기 위한 캐릭터 형식 변수 선언

    while( Serial.available() > 0 ) { // 입력된 값이 남아 있는 경우
      ch = Serial.read(); // 입력한 값을 불러와 ch에 저장
      str.concat(ch); // ch의 값을 str에 추가
      delay(10); // 0.01초 기다림
    }   
    return str; // 텍스트 값을 반환
}

void loop () // 무한 반복 함수
{
    String str; // 텍스트 형식 변수 선언
    str = readSerial(); // 함수를 호출해 시리얼 모니터에 입력한 값을 받아옴
    RtcDateTime now = Rtc.GetDateTime(); //RTC 모듈의 현재 시간 얻기
    if (str == ""); // 입력된 값이 없을 경우 패스
    else // 입력된 값이 있을 경우 
    {
      hour = (int(str[0])-48)*10 + int(str[1])-48; // 알람 시간을 아스키 코드로 받아들인 첫번째와 두번째 값에 48을 뺀 값(0의 아스키코드는 48, 1은 49 ...)을 더해서 숫자형으로 저장(첫번째 값은 십의 자리이므로 10을 곱함)
      minu = (int(str[3])-48)*10 + int(str[4])-48;// 알람 분을 아스키 코드로 받아들인 첫번째와 두번째 값에 48을 뺀 값(0의 아스키코드는 48, 1은 49 ...)을 더해서 숫자형으로 저장(첫번째 값은 십의 자리이므로 10을 곱함)
      Serial.print(hour); // 알람 시간 출력
      Serial.print("시 "); // 텍스트 출력
      Serial.print(minu); // 알람 분 출력
      Serial.println("분 예약되었습니다."); // 텍스트 출력
    }

    //시리얼 모니터에 출력
    if (nseco != now.Second()) { // 현재 초가 실제 초와 다른 경우
      nhour = now.Hour(); // 현재 시간을 실제 시간으로 설정
      nminu = now.Minute(); // 현재 분을 실제 분으로 설정
      nseco = now.Second(); // 현재 초를 실제 초로 설정
//      Serial.print("현재 시각은 ");
//      Serial.print(nhour);
//      Serial.print("시 ");
//      Serial.print(nminu);
//      Serial.print("분 ");
//      Serial.print(nseco);
//      Serial.println("초 입니다.");
    }

    if (hour == nhour && minu == nminu) { // 현재 시간과 분이 일치한 경우
      myservo.write(0); // servo의 각도 설정
      if (t == 1) t = millis(); // 랜덤 시간이 지났는지 측정하기 위한 신호를 받은 시간을 볼러오는 함수
      if (rd == -1) { // 시간을 랜덤으로 설정해야 하는 경우
        rd = random(8); // rd를 0~7까지 랜덤 값으로 설정
        rd = rd*1000; // rd에 1000을 곱함 (마이크로초 기준)
      }
      if (digitalRead(button) == HIGH && millis()-t < rd) { // 시간이 다 경과하지 않았는데 button을 누른 경우
        Serial.println("일찍 누르지 마세요."); // 텍스트 출력
        hour = -1; // 알람 초기화
        minu = -1; // 알람 초기화
        t = 1; // 시간 측정 변수 초기화
        rd = -1; // 랜덤 변수 초기화
      }
      if (millis()-t >= rd) { // 시간이 경과한 경우
        digitalWrite(buzzer, HIGH); // buzzer 작동
        digitalWrite(led, HIGH); // led 작동
        if (digitalRead(button) == HIGH) { // button이 눌린 경우
          pt = millis() - t; // 경과한 시간을 (현재 시간-작동하기 시작한 시간) 으로 설정
          pt = pt - rd; // 경과한 시간에서 랜덤으로 대기한 시간을 뺌
          pt = pt/1000; // 해당 시간을 1000으로 나눔 (마이크로 초이기 때문에 초로 환산)
          Serial.print("반응까지 "); // 텍스트 출력
          Serial.print(pt,10); // 반응 시간을 소수점 10의자리 까지 출력
          Serial.println("초 걸렸습니다."); // 텍스트 출력
          hour = -1; // 알람 초기화
          minu = -1; // 알람 초기화
          t = 1; // 시간 측정 변수 초기화
          rd = -1; // 랜덤 변수 초기화
        }
      }
    } else { // 알람에 맞춘 시간이 아닌 경우
      myservo.write(180); // servo의 각도 설정
      digitalWrite(buzzer, LOW); // buzzer 작동 중지
      digitalWrite(led, LOW); // led 작동 중지
    }
    
    //줄 바꿈.
    //Serial.println();
    //1초 대기...후 다시 loop 시작
}
/*
#define countof(a) (sizeof(a) / sizeof(a[0]))

//시리얼 모니터에 날짜 시간 표시하는 함수
void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
*/
