# RC LED 제어 시스템 (Arduino Uno & Radiolink R9D 수신기)

## 🌌 프로젝트 개요

이 프로젝트는 **Radiolink R9D 수신기**와 **Arduino Uno**를 이용하여 3채널 RC 시스템으로 아래 3가지 동작을 구현합니다:

1. **LED On/Off** (채널 1)
2. **LED 밝기 조절** (채널 2)
3. **RGB LED 색상 변경** (채널 3)

---

## 🌐 유튜브 시연 영상

> [시연 영상 보러가기](https://youtu.be/hRDZebeKj6o?si=OTun4FImhSfU06kO)

---

![KakaoTalk\_20250516\_164358389](https://github.com/user-attachments/assets/c08dc1f3-7101-4ed1-840f-d6fc703a21ce)

---

## 🔌 핀 연결 구성

### R9D → Arduino Uno

| R9D 채널 | Arduino 핀 | 기능 설명                |
| ------ | --------- | -------------------- |
| CH1    | A0        | LED On/Off (SWG 스위치) |
| CH2    | A1        | 밝기 조절                |
| CH3    | A2        | RGB 색상 전환            |
| VCC    | 5V        | 전원 공급                |
| GND    | GND       | 공통 접지                |

### Arduino 출력 핀

| 장치            | Arduino 핀 | 비고                      |
| ------------- | --------- | ----------------------- |
| LED1 (On/Off) | D9        | CH1에서 제어                |
| LED2 (밝기조절)   | D10       | CH2에서 제어                |
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
* 레버를 조작할때 LED가 켜졌다 꺼지는 것을 확인할 수 있음

### 2. LED 밝기 조절 (CH2 → D10)

```cpp
int brightness = map(ch2Pulse, 1000, 2000, 0, 255);
analogWrite(LED2_PIN, brightness);
```

* PWM 신호의 폭(1000~~2000us)을 0~~255로 매핑하여 `analogWrite()`로 출력
* 밝기 제어용 LED는 D10에 연결되며, PWM 신호로 아날로그 밝기 제어됨
* 조종기의 스틱 또는 노브를 위/아래로 움직이면 밝기가 점점 밝아지고 어두워짐

### 3. RGB 색상 전환 (CH3 → D5/D6/D3)

```cpp
if (ch3Pulse > 1700) {
    changeColor();
}
```

* CH3의 PWM 값이 일정 값 이상일 때 `changeColor()` 함수 실행 → 색상 순환 전환

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

* `setRGB()`는 D5(R), D6(G), D3(B)에 각각 PWM 신호를 주어 색을 혼합함
* 순차적으로 색이 바뀌며, 조종기의 스위치나 레버를 움직일 때마다 다른 색을 보여줌

---

## 📂 파일 구조

```
├── src
│   └── main.cpp            # Arduino 소스코드 (주석 포함)
├── README.md               # 프로젝트 설명 문서
```
---
