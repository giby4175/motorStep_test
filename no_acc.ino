const int stepPin = 2;
const int dirPin = 3;

void setup() {
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.println("Stepper Motor Ready. Enter number of steps:");
}

void loop() {
  int stepsToMove = 0;

  if (Serial.available() > 0) {
    // 讀取輸入的數字
    stepsToMove = Serial.parseInt();

    Serial.print("Moving steps: ");
    Serial.println(stepsToMove);

    // 控制方向
    digitalWrite(dirPin,HIGH);

    // 計算延遲
    float T_pulse = 1.0 / 1500; // s
    int delay_us = (T_pulse * 1e6) / 2; // 單邊

    // 執行步進
    if(stepsToMove < 0){
      digitalWrite(dirPin,LOW);
      stepsToMove -= 2*stepsToMove;
    }
    for (int x = 0; x < stepsToMove; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delay_us);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(delay_us);
    }
    digitalWrite(dirPin,HIGH);
    Serial.println("Done.");
  }
}