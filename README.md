# RC LED 제어 시스템 (Arduino Uno & Radiolink R9D 수신기)

## 🌌 프로젝트 개요

이 프로젝트는 **Radiolink R9D 수신기**와 **Arduino Uno**를 이용하여 3채널 RC 시스템으로 아래 3가지 동작을 구현합니다:

1. **LED On/Off** (채널 1)
2. **LED 밝기 조절** (채널 2)
3. **RGB LED 색상 변경** (채널 3)

---

## 🌐 유튜브 시연 영상

> [시연 영상 보러가기](https://youtu.be/YvAUqBdZ6mY)

---

## ⚡️ 회로도 구성

![KakaoTalk\_20250516\_164358389](https://github.com/user-attachments/assets/c08dc1f3-7101-4ed1-840f-d6fc703a21ce)

> ⚠️ 참고: 브레드보드 중앙에서 살짝 오른쪽 위의 빨간선은 해당 프로젝트와는 무관하므로 제거해도 됩니다.

---

## 🔌 핀 연결 구성

### R9D → Arduino Uno

| R9D 채널 | Arduino 핀 | 기능 설명                |
| ------ | --------- | -------------------- |
| CH1    | A0        | 밝기 조절  |
| CH2    | A1        | LED On/Off                |
| CH3    | A2        | RGB 색상 전환            |
| VCC    | 5V        | 전원 공급                |
| GND    | GND       | 공통 접지                |

### Arduino 출력 핀

| 장치            | Arduino 핀 | 비고                      |
| ------------- | --------- | ----------------------- |
| LED1 (On/Off) | D9        |  CH2에서 제어               |
| LED2 (밝기조절)   | D10       | CH1에서 제어                |
| RGB LED - 빨강  | D5        | analogWrite(PWM)로 색상 구성 |
| RGB LED - 초록  | D6        | analogWrite(PWM)로 색상 구성 |
| RGB LED - 파랑  | D3        | analogWrite(PWM)로 색상 구성 |

---

## ⚙️ 시스템 동작 설명

### 1. LED On/Off (CH1 → D9)

```cpp
if (ch1Pulse > 1600) {
    digitalWrite(LED1_PIN, HIGH); // 켜기
} else {
    digitalWrite(LED1_PIN, LOW);  // 끄기
}
```

* **레버**의 위치에 따라 PWM 값이 1600보다 크면 D9 핀에 HIGH 출력
* LED1은 HIGH 상태에서 켜지고 LOW 상태에서 꺼지므로, 스위치를 위/아래로 조작하여 LED On/Off 제어 가능

### 2. LED 밝기 조절 (CH2 → D10)

```cpp
int brightness = map(ch2Pulse, 1000, 2000, 0, 255);
analogWrite(LED2_PIN, brightness);
```

* 수신기의 PWM 폭(1000~~2000us)을 0~~255 밝기로 변환해 `analogWrite()`로 출력
* D10 핀은 PWM을 통해 LED2 밝기를 연속적으로 조절함
* 조종기의 **레버**을 위/아래로 움직이면 밝기 점점 증가/감소

### 3. RGB 색상 전환 (CH3 → D5/D6/D3)

```cpp
if (ch3Pulse > 1700) {
    changeColor();
}
```

* CH3의 PWM 신호가 일정 값 이상이면 `changeColor()` 함수 호출되어 색이 순차적으로 바뀜

```cpp
void changeColor() {
    currentColor = (currentColor + 1) % 6;
    switch (currentColor) {
        case 0: setRGB(255, 0, 0); break;   // 빨강
        case 1: setRGB(0, 255, 0); break;   // 초록
        case 2: setRGB(0, 0, 255); break;   // 파랑
        case 3: setRGB(255, 255, 0); break; // 노랑
        case 4: setRGB(0, 255, 255); break; // 시안
        case 5: setRGB(255, 0, 255); break; // 마젠타
    }
}
```

* `setRGB()` 함수는 각각의 RGB 핀(D5/D6/D3)에 PWM 신호를 출력하여 색을 혼합 생성
* 조종기의 레버 조작 시 6가지 색상이 순차적으로 출력됨

---

### ✨ 핀체인지 인터럽트 처리 원리

PWM 신호는 \*\*상승 에지(HIGH)\*\*와 \*\*하강 에지(LOW)\*\*를 감지하여 신호의 폭(길이)을 측정합니다.
핀체인지 인터럽트를 통해 아래와 같은 방식으로 처리됩니다:

```cpp
// === 인터럽트 핸들러: 채널 1 ===
void handleCH1() {
  if (digitalRead(CH1_PIN) == HIGH)
    ch1Start = micros(); // 상승 에지 시간 저장
  else if (ch1Start && !newCh1) {
    ch1Pulse = micros() - ch1Start; // 하강 에지에서 펄스폭 계산
    ch1Start = 0;
    newCh1 = true; // loop()에서 처리 가능하도록 플래그 설정
  }
}
```

* 동일한 방식으로 CH2, CH3도 각각 인터럽트로 측정합니다.
* loop()에서는 `newChX`가 true일 때만 새로운 입력으로 반응하여 동작을 수행함

---

## 📂 파일 구조

```
├── src
│   └── main.cpp            # Arduino 소스코드 (주석 포함)
├── README.md               # 프로젝트 설명 문서
```

---

