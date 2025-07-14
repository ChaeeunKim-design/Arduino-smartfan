// 스마트 펫 선풍기 - 아두이노 통합 코드

// 핀 정의
const int tempPin = A0;         // LM35 온도 센서
const int irSensorPin = 2;      // TCRT5000 근접 센서
const int buttonPin = 9;        // 수동 푸시버튼 입력
const int in1 = 4;              // 스텝모터 IN1
const int in2 = 5;              // 스텝모터 IN2
const int in3 = 6;              // 스텝모터 IN3
const int in4 = 7;              // 스텝모터 IN4

// 상태 변수들
bool fanState = false;          // 팬 ON/OFF 상태
bool lastButtonState = HIGH;    // 버튼 이전 상태
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // 디바운싱 지연

void setup() {
  Serial.begin(9600);           // 시리얼 통신 시작

  pinMode(irSensorPin, INPUT);
  pinMode(buttonPin, INPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  stopFan();  // 시작 시 팬 꺼짐 상태로 초기화
}

void loop() {
  // 1. 온도 측정
  int tempRaw = analogRead(tempPin);
  float temperature = tempRaw * 0.488; // LM35 계산식 (5V 기준)

  // 2. 거리 감지
  int irState = digitalRead(irSensorPin); // LOW = 가까움, HIGH = 멀리 있음

  // 3. 버튼 입력 처리 (토글)
  bool currentButtonState = digitalRead(buttonPin);
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentButtonState == LOW && lastButtonState == HIGH) {
      fanState = !fanState;  // 버튼 눌림 → 상태 토글
    }
  }
  lastButtonState = currentButtonState;

  // 4. 라즈베리파이로부터 시리얼 명령 수신
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "ON") fanState = true;
    else if (cmd == "OFF") fanState = false;
  }

  // 5. 조건 확인 및 팬 상태 결정
  bool shouldTurnOn = fanState && (temperature > 25.0) && (irState == HIGH);

  if (shouldTurnOn) {
    runFan();  // 팬 작동
  } else {
    stopFan(); // 팬 정지
  }

  // 6. 디버깅 출력
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" | IR: "); Serial.print(irState);
  Serial.print(" | Fan: "); Serial.println(shouldTurnOn ? "ON" : "OFF");

  delay(100); // 짧은 딜레이
}

// 스텝모터 회전 함수 예시 (단순 시계방향)
void runFan() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopFan() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
