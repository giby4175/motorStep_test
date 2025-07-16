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
    if (stepsToMove < 0) {
      digitalWrite(dirPin, LOW);
      stepsToMove = -stepsToMove; // 絕對值
    } else {
      digitalWrite(dirPin, HIGH);
    }

    // 區段比例
    int stepsToAcc = stepsToMove / 6;
    int stepsFixed = stepsToMove * 2 / 3;

    // 線性頻率規劃
    float f_start = 300; // Hz，啟動頻率
    float f_end = 1050;   // Hz，最高頻率（等速）
    float slope = (f_end - f_start) / stepsToAcc;

    // === 加速段 ===
    for (int i = 0; i < stepsToAcc; i++) {
      float f_step = f_start + slope * i;         // 線性頻率
      float T_step = 1.0 / f_step;                // 秒
      int delay_us = (T_step * 1e6) / 2;          // 半週期

      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delay_us);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(delay_us);

    }

    // === 等速段 ===
    float T_fixed = 1.0 / f_end;
    int delay_fixed_us = (T_fixed * 1e6) / 2;

    for (int x = 0; x < stepsFixed; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delay_fixed_us);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(delay_fixed_us);
    }

    // === 減速段 ===
    for (int i = 0; i < stepsToAcc; i++) {
      float f_step = f_end - slope * i;           // 線性頻率降低
      float T_step = 1.0 / f_step;                // 秒
      int delay_us = (T_step * 1e6) / 2;          // 半週期

      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delay_us);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(delay_us);

    }
    
    Serial.println("Done.");
  }
}
