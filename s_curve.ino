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

    // 最高速度、起始速度規劃
    float f_start = 300; // Hz，啟動頻率
    float f_end = 1500;   // Hz，最高頻率（等速）

    // 決定s-curve內的比例
    int stepsJerkUp = stepsToAcc * 0.2;
    int stepsJerkConst = stepsToAcc * 0.6;
    int stepsJerkDown = stepsToAcc * 0.2;

    // 用理論推導算出 Amax & Jmax
    float v_end_sq = f_end * f_end;
    float v_start_sq = f_start * f_start;
    float Amax = (v_end_sq - v_start_sq) / (2.0 * stepsToAcc);
    float jerk_up_steps = stepsJerkUp;
    float avg_pulse_time = 1.0 / ((f_start + f_end) / 2.0);
    float T_jerk_up = jerk_up_steps * avg_pulse_time;
    float Jmax = Amax / T_jerk_up;

    float f_cur = f_start;
    float a_cur = 0.0;
    float j_cur = 0.0;

    // === 加速段 ===
    for (int i = 0; i < stepsToAcc; i++) {

      // 決定 jerk
      if (i < stepsJerkUp) {
        j_cur = Jmax;
      } else if (i < stepsJerkUp + stepsJerkConst) {
        j_cur = 0.0;
      } else {
        j_cur = -Jmax;
      }

      // 更新加速度 & 頻率
      a_cur += j_cur * avg_pulse_time;
      f_cur += a_cur * avg_pulse_time;

      // 限制 f_cur
      if (f_cur > f_end) f_cur = f_end;

      // 計算延遲
      float T_pulse = 1.0 / f_cur; // s
      int delay_us = (T_pulse * 1e6) / 2; // 單邊

      // 發脈衝
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
    f_cur = f_end;
    a_cur = 0.0;
    j_cur = 0.0;

    for (int i = 0; i < stepsToAcc; i++) {
      if (i < stepsJerkUp) {
        j_cur = -Jmax;  // 減速
      } else if (i < stepsJerkUp + stepsJerkConst) {
        j_cur = 0.0;
      } else {
        j_cur = +Jmax;
      }

      a_cur += j_cur * avg_pulse_time;
      f_cur += a_cur * avg_pulse_time;

      if (f_cur < f_start) f_cur = f_start;

      float T_pulse = 1.0 / f_cur;
      int delay_us = (T_pulse * 1e6) / 2;

      digitalWrite(stepPin, HIGH);
      delayMicroseconds(delay_us);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(delay_us);
    }
    
    Serial.println("Done.");
  }
}
