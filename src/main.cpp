#include <Arduino.h>
#include <PinChangeInterrupt.h> // 핀체인지 인터럽트를 위한 라이브러리

// 수신기에서 들어오는 채널 핀 정의
#define CH1_PIN A0  // 채널 1: LED On/Off 제어용 (스위치)
#define CH2_PIN A1  // 채널 2: LED 밝기 제어용
#define CH3_PIN A2  // 채널 3: RGB 색상 전환용

// 아두이노 출력 핀 정의
const int LED1_PIN = 9;    // 일반 LED 1: ON/OFF
const int LED2_PIN = 10;   // 일반 LED 2: 밝기 조절
const int RGB_R = 5;       // RGB LED의 빨간색 핀
const int RGB_G = 6;       // 초록색 핀
const int RGB_B = 3;       // 파란색 핀

// 각 채널에서 읽은 PWM 신호값 저장 변수 (us 단위)
volatile int ch1Pulse = 1500;
volatile int ch2Pulse = 1500;
volatile int ch3Pulse = 1500;

// PWM 신호 측정을 위한 상승 에지 시작 시간 기록 변수
volatile unsigned long ch1Start = 0;
volatile unsigned long ch2Start = 0;
volatile unsigned long ch3Start = 0;

// 새로운 PWM 신호가 들어왔는지 확인하는 플래그
volatile boolean newCh1 = false;
volatile boolean newCh2 = false;
volatile boolean newCh3 = false;

// 현재 RGB 색상 상태 (0~5 순환)
int currentColor = 0;

// 함수 미리 선언
void handleCH1();
void handleCH2();
void handleCH3();
void changeColor();
void setRGB(int r, int g, int b);

void setup() {
  Serial.begin(9600); // 시리얼 통신 시작 (모니터 출력용)
  delay(1000);
  Serial.println("🎛 RC 3채널 LED 제어 시작");

  // 입력 핀 설정 (수신기 PWM 입력 받음)
  pinMode(CH1_PIN, INPUT_PULLUP);
  pinMode(CH2_PIN, INPUT_PULLUP);
  pinMode(CH3_PIN, INPUT_PULLUP);

  // 출력 핀 설정 (LED 제어)
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  // 핀 체인지 인터럽트 등록 (PWM 신호 측정용)
  attachPCINT(digitalPinToPCINT(CH1_PIN), handleCH1, CHANGE);
  attachPCINT(digitalPinToPCINT(CH2_PIN), handleCH2, CHANGE);
  attachPCINT(digitalPinToPCINT(CH3_PIN), handleCH3, CHANGE);
}

void loop() {
  // === CH1 처리: LED ON/OFF 제어 ===
  if (newCh1) {
    newCh1 = false;

    // PWM 신호가 1600us 초과면 LED 켜기, 아니면 끄기
    if (ch1Pulse > 1600) {
      digitalWrite(LED1_PIN, HIGH);
    } else {
      digitalWrite(LED1_PIN, LOW);
    }

    Serial.print("[CH1] PWM: ");
    Serial.println(ch1Pulse);
  }

  // === CH2 처리: 밝기 제어 ===
  if (newCh2) {
    newCh2 = false;

    // 1000~2000us PWM을 0~255 밝기로 매핑
    int brightness = map(ch2Pulse, 1000, 2000, 0, 255);
    brightness = constrain(brightness, 0, 255); // 범위 제한
    analogWrite(LED2_PIN, brightness); // PWM 출력

    Serial.print("[CH2] PWM: ");
    Serial.println(ch2Pulse);
  }

  // === CH3 처리: RGB 색상 전환 ===
  if (newCh3) {
    newCh3 = false;

    // 스틱/스위치가 특정 위치 이상이면 색상 변경
    if (ch3Pulse > 1700) {
      changeColor(); // 다음 색상으로 변경
    }

    Serial.print("[CH3] PWM: ");
    Serial.println(ch3Pulse);
  }

  delay(50); // 시리얼 출력 속도 제어 (너무 빠르지 않게)
}

// === 인터럽트 핸들러: 채널 1 ===
void handleCH1() {
  if (digitalRead(CH1_PIN) == HIGH)
    ch1Start = micros(); // 상승 에지 시간 저장
  else if (ch1Start && !newCh1) {
    ch1Pulse = micros() - ch1Start; // 하강 에지에서 펄스폭 계산
    ch1Start = 0;
    newCh1 = true;
  }
}

// === 인터럽트 핸들러: 채널 2 ===
void handleCH2() {
  if (digitalRead(CH2_PIN) == HIGH)
    ch2Start = micros();
  else if (ch2Start && !newCh2) {
    ch2Pulse = micros() - ch2Start;
    ch2Start = 0;
    newCh2 = true;
  }
}

// === 인터럽트 핸들러: 채널 3 ===
void handleCH3() {
  if (digitalRead(CH3_PIN) == HIGH)
    ch3Start = micros();
  else if (ch3Start && !newCh3) {
    ch3Pulse = micros() - ch3Start;
    ch3Start = 0;
    newCh3 = true;
  }
}

// === RGB 색상 전환 ===
void changeColor() {
  currentColor = (currentColor + 1) % 6; // 6가지 색상 반복
  switch (currentColor) {
    case 0: setRGB(255, 0, 0); break;   // 빨강
    case 1: setRGB(0, 255, 0); break;   // 초록
    case 2: setRGB(0, 0, 255); break;   // 파랑
    case 3: setRGB(255, 255, 0); break; // 노랑
    case 4: setRGB(0, 255, 255); break; // 시안
    case 5: setRGB(255, 0, 255); break; // 마젠타
  }
}

// === RGB 값 PWM 출력 ===
void setRGB(int r, int g, int b) {
  analogWrite(RGB_R, r); // R 핀에 PWM
  analogWrite(RGB_G, g); // G 핀에 PWM
  analogWrite(RGB_B, b); // B 핀에 PWM
}
