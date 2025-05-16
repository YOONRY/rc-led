#include <Arduino.h>
#include <PinChangeInterrupt.h>

// 채널 핀
#define CH1_PIN A0
#define CH2_PIN A1
#define CH3_PIN A2

// LED 출력 핀
const int LED1_PIN = 9;    // On/Off용 LED
const int LED2_PIN = 10;   // 밝기 조절 LED
const int RGB_R = 5;
const int RGB_G = 6;
const int RGB_B = 3;

// PWM 측정 변수
volatile int ch1Pulse = 1500;
volatile int ch2Pulse = 1500;
volatile int ch3Pulse = 1500;

volatile unsigned long ch1Start = 0;
volatile unsigned long ch2Start = 0;
volatile unsigned long ch3Start = 0;

volatile boolean newCh1 = false;
volatile boolean newCh2 = false;
volatile boolean newCh3 = false;

// RGB 색상 상태
int currentColor = 0;

// 함수 선언
void handleCH1();
void handleCH2();
void handleCH3();
void changeColor();
void setRGB(int r, int g, int b);

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("🎛 RC 3채널 LED 제어 시작");

  // 입력 핀 설정
  pinMode(CH1_PIN, INPUT_PULLUP);
  pinMode(CH2_PIN, INPUT_PULLUP);
  pinMode(CH3_PIN, INPUT_PULLUP);

  // 출력 핀 설정
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  // 핀체인지 인터럽트 등록
  attachPCINT(digitalPinToPCINT(CH1_PIN), handleCH1, CHANGE);
  attachPCINT(digitalPinToPCINT(CH2_PIN), handleCH2, CHANGE);
  attachPCINT(digitalPinToPCINT(CH3_PIN), handleCH3, CHANGE);
}

void loop() {
  // CH1 → LED1 ON/OFF
  if (newCh1) {
    newCh1 = false;
    if (ch1Pulse > 1600) {
      digitalWrite(LED1_PIN, HIGH);
    } else {
      digitalWrite(LED1_PIN, LOW);
    }
    Serial.print("[CH1] PWM: "); Serial.println(ch1Pulse);
  }

  // CH2 → LED2 밝기 조절
  if (newCh2) {
    newCh2 = false;
    int brightness = map(ch2Pulse, 1000, 2000, 0, 255);
    brightness = constrain(brightness, 0, 255);
    analogWrite(LED2_PIN, brightness);
    Serial.print("[CH2] PWM: "); Serial.println(ch2Pulse);
  }

  // CH3 → RGB 색상 전환
  if (newCh3) {
    newCh3 = false;
    if (ch3Pulse > 1700) {
      changeColor();
    }
    Serial.print("[CH3] PWM: "); Serial.println(ch3Pulse);
  }

  delay(50); // 출력 속도 조절
}

// --- 인터럽트 핸들러 ---
void handleCH1() {
  if (digitalRead(CH1_PIN) == HIGH)
    ch1Start = micros();
  else if (ch1Start && !newCh1) {
    ch1Pulse = micros() - ch1Start;
    ch1Start = 0;
    newCh1 = true;
  }
}

void handleCH2() {
  if (digitalRead(CH2_PIN) == HIGH)
    ch2Start = micros();
  else if (ch2Start && !newCh2) {
    ch2Pulse = micros() - ch2Start;
    ch2Start = 0;
    newCh2 = true;
  }
}

void handleCH3() {
  if (digitalRead(CH3_PIN) == HIGH)
    ch3Start = micros();
  else if (ch3Start && !newCh3) {
    ch3Pulse = micros() - ch3Start;
    ch3Start = 0;
    newCh3 = true;
  }
}

// --- RGB 색상 순환 ---
void changeColor() {
  currentColor = (currentColor + 1) % 6;
  switch (currentColor) {
    case 0: setRGB(255, 0, 0); break;   // Red
    case 1: setRGB(0, 255, 0); break;   // Green
    case 2: setRGB(0, 0, 255); break;   // Blue
    case 3: setRGB(255, 255, 0); break; // Yellow
    case 4: setRGB(0, 255, 255); break; // Cyan
    case 5: setRGB(255, 0, 255); break; // Magenta
  }
}

// --- RGB 아날로그 출력 ---
void setRGB(int r, int g, int b) {
  analogWrite(RGB_R, r);
  analogWrite(RGB_G, g);
  analogWrite(RGB_B, b);
}
