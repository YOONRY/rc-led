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

![KakaoTalk_20250516_164358389](https://github.com/user-attachments/assets/c08dc1f3-7101-4ed1-840f-d6fc703a21ce)


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

| 장치            | Arduino 핀 | 비고                    |
| ------------- | --------- | --------------------- |
| LED1 (On/Off) | D9        | CH1에서 제어              |
| LED2 (밝기조절)   | D10       | CH2에서 제어 |
| RGB LED - 빨강  | D5        |                       |
| RGB LED - 초록  | D6        |                       |
| RGB LED - 파랑  | D3        |                       |

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

* 조종기의 **SWG 스위치** 아래로 내렸을 때 LED가 켜짐

### 2. LED 밝기 조절 (CH2 → D10)

```cpp
int brightness = map(ch2Pulse, 1000, 2000, 0, 255);
analogWrite(LED2_PIN, brightness);
```

* 스틱 또는 노브 위치에 따라 밝기 연속 조절

### 3. RGB 색상 전환 (CH3 → D5/D6/D3)

```cpp
if (ch3Pulse > 1700) {
    changeColor();
}
```

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

---

## 📂 파일 구조

```
├── src
│   └── main.cpp            # Arduino 소스코드 (주석 포함)
├── README.md               # 프로젝트 설명 문서
```

---

## 📊 재현을 위한 참고 사항

* **R9D와 Arduino의 GND를 반드시 연결**할 것
* **공통 캐소드 RGB LED 사용 권장**
* **LED 출력 핀에는 220Ω\~330Ω 저항 연결**
* **수신기 전원은 Arduino 5V 사용 가능하나 안정된 전원 권장**

---
```
