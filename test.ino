//00000001

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

    // 執行步進
    if(stepsToMove < 0){
      digitalWrite(dirPin,LOW);
      stepsToMove -= 2*stepsToMove;
    }
    for (int x = 0; x < stepsToMove; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(1000);
    }
    digitalWrite(dirPin,HIGH);
    Serial.println("Done.");
  }
}
