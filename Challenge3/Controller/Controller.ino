//#include <Arduino.h>
//
//#define X_STEP_PIN     2
//#define X_DIR_PIN      5
//#define ENABLE_PIN     8
//#define LED_PIN        A3
//#define V_5            A0
//#define LED_MOTION_PIN 11
//#define SENSOR_PIN_1   12
//#define SENSOR_PIN_2   13
//
//const int STEPS_PER_REV = 3200;
//const float STEPS_PER_DEGREE = STEPS_PER_REV / 360.0f;
//const int RPM = 1;
//const unsigned long STEP_PERIOD_US = 60000000UL / (STEPS_PER_REV * RPM);
//const unsigned long ZERO_TIMEOUT = 60000; 
//const int ZERO_THRESHOLD = 50; 
//
//int currentStep = 0;
//unsigned long lastZeroTime = 0;
//int zeroPassCount = 0;
//bool ledBlinkState = false;
//
//void setup() {
//  Serial.begin(9600);
//  while (!Serial);  
//  pinMode(X_STEP_PIN, OUTPUT);
//  pinMode(X_DIR_PIN, OUTPUT);
//  pinMode(ENABLE_PIN, OUTPUT);
//  pinMode(LED_PIN, OUTPUT);
//  pinMode(LED_MOTION_PIN, OUTPUT);
//  pinMode(V_5, OUTPUT);
//  pinMode(SENSOR_PIN_1, INPUT);
//  pinMode(SENSOR_PIN_2, INPUT);
//  digitalWrite(ENABLE_PIN, LOW); 
//  analogWrite(V_5,255);
//  analogWrite(LED_PIN, 255); 
//
//  Serial.println("System Ready");
//}
//
//void loop() {
//  if (digitalRead(SENSOR_PIN_1) == LOW || digitalRead(SENSOR_PIN_2) == LOW) {
//    digitalWrite(LED_MOTION_PIN, HIGH);
//  } else {
//    digitalWrite(LED_MOTION_PIN, LOW);
//  }
//
//  checkZeroPassEvent();
//
//  if (Serial.available()) {
//    String input = Serial.readStringUntil('\n');
//    input.trim();  
//
//    if (input.length() == 0) return;
//
//    Serial.print("Received: ");
//    analogWrite(LED_PIN, 0); 
//    Serial.println(input);
//
//    if (input.startsWith("COORD:")) {
//      handleCoord(input.substring(6).c_str());  
//    } else {
//      Serial.println("Unknown command");
//    }
//
//    Serial.println("Done");
//    analogWrite(LED_PIN, 255); 
//  }
//}
//
//void handleCoord(const char* coordData) {
//  String coordStr = String(coordData);
//  coordStr.trim();
//
//  int commaIndex = coordStr.indexOf(',');
//  if (commaIndex != -1) {
//    String xStr = coordStr.substring(0, commaIndex);
//    String yStr = coordStr.substring(commaIndex + 1);
//    xStr.trim();
//    yStr.trim();
//
//    float x = xStr.toFloat();
//    float y = yStr.toFloat();
//
//    rotateToCoord(x, y);
//  } else {
//    Serial.println("Invalid COORD format. Use: COORD:x,y");
//  }
//}
//
//bool checkObstacle() {
//  return (digitalRead(SENSOR_PIN_1) == LOW || digitalRead(SENSOR_PIN_2) == LOW);
//}
//
//void rotateToCoord(float x, float y) {
//  float angle = atan2(y, x) * 180.0f / PI;
//  if (angle < 0) angle += 360.0f;
//
//  int targetStep = round(angle * STEPS_PER_DEGREE);
//  int stepsToMove = targetStep - currentStep;
//  
//  if (abs(stepsToMove) > STEPS_PER_REV / 2) {
//    stepsToMove = stepsToMove > 0 ? stepsToMove - STEPS_PER_REV : stepsToMove + STEPS_PER_REV;
//  }
//
//  int stepCount = abs(stepsToMove);
//  int stepDirection = (stepsToMove >= 0) ? 1 : -1;
//
//  digitalWrite(X_DIR_PIN, stepsToMove >= 0 ? HIGH : LOW);
//
//  if (stepCount == 0) {
//    Serial.println("Already at target angle, no movement needed");
//    return;
//  }
//  
//  int stepsMoved = 0;
//  for (int i = 0; i < stepCount; i++) {
//    if (checkObstacle()) {
//      Serial.println("Obstacle detected! Stopping rotation.");
//      digitalWrite(LED_MOTION_PIN, HIGH);
//      
//      currentStep += stepsMoved * stepDirection;
////      Serial.print("Partial move: ");
////      Serial.print(stepsMoved);
////      Serial.println(" steps");
//      return;
//    }
//    
//    digitalWrite(X_STEP_PIN, HIGH);
//    delayMicroseconds(10);
//    digitalWrite(X_STEP_PIN, LOW);
//    delayMicroseconds(STEP_PERIOD_US - 10);
//    
//    stepsMoved++;
//  }
//
//  currentStep += stepsMoved * stepDirection;
//}
//
//void checkZeroPassEvent() {
//  bool isNearZero = (abs(currentStep % STEPS_PER_REV) <= ZERO_THRESHOLD) || 
//                   (abs(currentStep % STEPS_PER_REV - STEPS_PER_REV) <= ZERO_THRESHOLD);
//
//  if (isNearZero) {
//    unsigned long currentTime = millis();
//    
//    if (currentTime - lastZeroTime < ZERO_TIMEOUT) {
//      zeroPassCount++;
////      Serial.print("Passed zero position. Count: ");
////      Serial.println(zeroPassCount);
//    } else {
//      zeroPassCount = 1;
//    }
//    
//    lastZeroTime = currentTime;
//
//    if (zeroPassCount >= 2) {
//      blinkLED();
//    }
//  }
//}
//
//void blinkLED() {
//  static unsigned long lastBlinkTime = 0;
//  const unsigned long BLINK_INTERVAL = 500; 
//  
//  unsigned long currentTime = millis();
//  
//  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
//    lastBlinkTime = currentTime;
//    ledBlinkState = !ledBlinkState;
//    analogWrite(LED_PIN, ledBlinkState ? 255 : 0);
//    
//    if (currentTime - lastZeroTime > ZERO_TIMEOUT) {
//      zeroPassCount = 0;
//      analogWrite(LED_PIN, 255); 
//    }
//  }
//}


//
//#include <Arduino.h>
//
//#define X_STEP_PIN     2
//#define X_DIR_PIN      5
//#define ENABLE_PIN     8
//#define LED_PIN        A3
//#define V_5            A0
//#define LED_MOTION_PIN 11
//#define SENSOR_PIN_1   12
//#define SENSOR_PIN_2   13
//
//const int STEPS_PER_REV = 3200;
//const float PULLEY_DIAMETER = 13.0;       
//const float BELT_PITCH = 1.0;              
//const float STEPS_PER_MM = STEPS_PER_REV / (PI * PULLEY_DIAMETER);
//const float MAX_DISTANCE_MM = 610.0;       
//const long MAX_POSITION_STEPS = round(MAX_DISTANCE_MM * STEPS_PER_MM);
//const int RPM = 150;                       
//const unsigned long STEP_PERIOD_US = 60000000UL / (STEPS_PER_REV * RPM);
//
//long currentPosition = 0;  
//const int EDGE_THRESHOLD = 50; 
//const unsigned long EDGE_TIMEOUT = 60000; 
//
//unsigned long lastEdgeTime = 0;
//int edgePassCount = 0;
//bool ledBlinkState = false;
//
//const float CALIBRATION_FACTOR = 1;  
//
//void setup() {
//  Serial.begin(9600);
//  while (!Serial);  
//  pinMode(X_STEP_PIN, OUTPUT);
//  pinMode(X_DIR_PIN, OUTPUT);
//  pinMode(ENABLE_PIN, OUTPUT);
//  pinMode(LED_PIN, OUTPUT);
//  pinMode(LED_MOTION_PIN, OUTPUT);
//  pinMode(V_5, OUTPUT);
//  pinMode(SENSOR_PIN_1, INPUT);
//  pinMode(SENSOR_PIN_2, INPUT);
//  
//  digitalWrite(ENABLE_PIN, LOW); 
//  analogWrite(V_5, 255);
//  analogWrite(LED_PIN, 255); 
//
//  Serial.println("System Ready");
////  Serial.print("Steps per mm: ");
////  Serial.println(STEPS_PER_MM, 4);
////  Serial.print("Max Position Steps: ");
////  Serial.println(MAX_POSITION_STEPS);
//}
//
//void loop() {
//  if (checkObstacle()) {
//    digitalWrite(LED_MOTION_PIN, HIGH);
//  } else {
//    digitalWrite(LED_MOTION_PIN, LOW);
//  }
//
//  checkEdgeEvent();
//
//  if (Serial.available()) {
//    String input = Serial.readStringUntil('\n');
//    input.trim();  
//
//    if (input.length() == 0) return;
//
//    Serial.print("Received: ");
//    analogWrite(LED_PIN, 0); 
//    Serial.println(input);
//
//    if (input.startsWith("COORD:")) {
//      handleCoord(input.substring(6).c_str());  
//    } else if (input.equalsIgnoreCase("HOME")) {
//      homePosition();
//    } else if (input.equalsIgnoreCase("TEST")) {
//      testFullDistance();
//    } else if (input.equalsIgnoreCase("WHERE")) {
//      reportCurrentPosition();
//    } else {
//      Serial.println("Unknown command");
//    }
//
//    Serial.println("Done");
//    analogWrite(LED_PIN, 255); 
//  }
//}
//
//bool checkObstacle() {
//  return (digitalRead(SENSOR_PIN_1) == LOW || digitalRead(SENSOR_PIN_2) == LOW);
//}
//
//void handleCoord(const char* coordData) {
//  String coordStr = String(coordData);
//  coordStr.trim();
//
//  int commaIndex = coordStr.indexOf(',');
//  if (commaIndex != -1) {
//    String xStr = coordStr.substring(0, commaIndex);
//    String yStr = coordStr.substring(commaIndex + 1);
//    xStr.trim();
//    yStr.trim();
//
//    float x = xStr.toFloat();
//    
//    x *= CALIBRATION_FACTOR;
//    
//    moveToPosition(x);
//  } else {
//    Serial.println("Invalid COORD format. Use: COORD:x,y");
//  }
//}
//
//void moveToPosition(float targetX) {
//  long targetPosition = round(targetX * STEPS_PER_MM);
//  
//  if (targetPosition < 0) targetPosition = 0;
//  if (targetPosition > MAX_POSITION_STEPS) targetPosition = MAX_POSITION_STEPS;
//  
//  long stepsToMove = targetPosition - currentPosition;
//  
//  if (stepsToMove == 0) {
//    Serial.println("Already at target position, no movement needed");
//    return;
//  }
//  
//  int stepDirection = (stepsToMove >= 0) ? 1 : -1;
//  digitalWrite(X_DIR_PIN, stepsToMove >= 0 ? HIGH : LOW);
//  
//  long stepCount = abs(stepsToMove);
//  long stepsMoved = 0;
//  
////  Serial.print("Moving from ");
////  Serial.print(currentPosition / STEPS_PER_MM);
////  Serial.print("mm to ");
////  Serial.print(targetX);
////  Serial.print("mm (");
////  Serial.print(stepCount);
////  Serial.println(" steps)");
//  
//  for (long i = 0; i < stepCount; i++) {
//    if (digitalRead(SENSOR_PIN_1) == LOW || digitalRead(SENSOR_PIN_2) == LOW) {
////      Serial.println("EMERGENCY STOP! Movement halted.");
//      digitalWrite(LED_MOTION_PIN, HIGH);
//      currentPosition += stepsMoved * stepDirection; 
////      Serial.print("Stopped at ");
////      Serial.print(currentPosition / STEPS_PER_MM);
////      Serial.println("mm");
//      return;
//    }
//    
//    digitalWrite(X_STEP_PIN, HIGH);
//    delayMicroseconds(10);
//    digitalWrite(X_STEP_PIN, LOW);
//    delayMicroseconds(STEP_PERIOD_US);
//    
//    stepsMoved++;
//  }
//  
//  currentPosition += stepsMoved * stepDirection;
////  Serial.print("Moved to position: ");
////  Serial.print(currentPosition);
////  Serial.print(" steps (");
////  Serial.print(currentPosition / STEPS_PER_MM);
////  Serial.println(" mm)");
//}
//
//void homePosition() {
////  Serial.println("Moving to home position (0mm)");
//  moveToPosition(0.0);
//  currentPosition = 0; 
////  Serial.println("Homing complete");
//}
//
//void testFullDistance() {
////  Serial.println("Testing full distance (610mm)");
//  moveToPosition(610.0);
//}
//
//void reportCurrentPosition() {
//  Serial.print("Current position: ");
//  Serial.print(currentPosition);
//  Serial.print(" steps (");
//  Serial.print(currentPosition / STEPS_PER_MM);
//  Serial.println(" mm)");
//}
//
//void checkEdgeEvent() {
//  bool nearLeftEdge = (currentPosition <= EDGE_THRESHOLD);
//  bool nearRightEdge = (currentPosition >= (MAX_POSITION_STEPS - EDGE_THRESHOLD));
//  
//  if (nearLeftEdge || nearRightEdge) {
//    unsigned long currentTime = millis();
//    
//    if (currentTime - lastEdgeTime < EDGE_TIMEOUT) {
//      edgePassCount++;
////      Serial.print("Passed edge position. Count: ");
////      Serial.println(edgePassCount);
//    } else {
//      edgePassCount = 1;
//    }
//    
//    lastEdgeTime = currentTime;
//
//    if (edgePassCount >= 2) {
//      blinkAlertLED();
//    }
//  } else {
//    edgePassCount = 0;
//  }
//}
//
//void blinkAlertLED() {
//  static unsigned long lastBlinkTime = 0;
//  const unsigned long BLINK_INTERVAL = 500; 
//  
//  unsigned long currentTime = millis();
//  
//  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
//    lastBlinkTime = currentTime;
//    ledBlinkState = !ledBlinkState;
//    analogWrite(LED_PIN, ledBlinkState ? 255 : 0);
//  }
//  
//  if (currentTime - lastEdgeTime > EDGE_TIMEOUT) {
//    edgePassCount = 0;
//    analogWrite(LED_PIN, 255); 
//  }
//}



//
//
//#include <Arduino.h>
//
//#define X_STEP_PIN     2
//#define X_DIR_PIN      5
//#define ENABLE_PIN     8
//#define LED_PIN        A3
//#define V_5            A0
//#define LED_MOTION_PIN 11
//#define SENSOR_PIN_1   12
//#define SENSOR_PIN_2   13
//
//const int STEPS_PER_REV = 3200;
//const float PULLEY_DIAMETER = 13.0;       
//const float BELT_PITCH = 1.0;              
//const float STEPS_PER_MM = STEPS_PER_REV / (PI * PULLEY_DIAMETER);
//const float MAX_DISTANCE_MM = 610.0;       
//const long MAX_POSITION_STEPS = round(MAX_DISTANCE_MM * STEPS_PER_MM);
//const int RPM = 150;                       
//const unsigned long STEP_PERIOD_US = 60000000UL / (STEPS_PER_REV * RPM);
//
//const int OBSTACLE_CONFIRM_COUNT = 5; 
//
//long currentPosition = 0;  
//const int EDGE_THRESHOLD = 50; 
//const unsigned long EDGE_TIMEOUT = 60000; 
//
//unsigned long lastEdgeTime = 0;
//int edgePassCount = 0;
//bool ledBlinkState = false;
//
//const float CALIBRATION_FACTOR = 1;  
//
//void setup() {
//  Serial.begin(9600);
//  while (!Serial);  
//  pinMode(X_STEP_PIN, OUTPUT);
//  pinMode(X_DIR_PIN, OUTPUT);
//  pinMode(ENABLE_PIN, OUTPUT);
//  pinMode(LED_PIN, OUTPUT);
//  pinMode(LED_MOTION_PIN, OUTPUT);
//  pinMode(V_5, OUTPUT);
//  pinMode(SENSOR_PIN_1, INPUT);
//  pinMode(SENSOR_PIN_2, INPUT);
//  
//  digitalWrite(ENABLE_PIN, LOW); 
//  analogWrite(V_5, 255);
//  analogWrite(LED_PIN, 255); 
//
//  Serial.println("System Ready");
//}
//
//void loop() {
//  if (checkObstacle()) {
//    digitalWrite(LED_MOTION_PIN, HIGH);
//  } else {
//    digitalWrite(LED_MOTION_PIN, LOW);
//  }
//
//  checkEdgeEvent();
//
//  if (Serial.available()) {
//    String input = Serial.readStringUntil('\n');
//    input.trim();  
//
//    if (input.length() == 0) return;
//
//    Serial.print("Received: ");
//    analogWrite(LED_PIN, 0); 
//    Serial.println(input);
//
//    if (input.startsWith("COORD:")) {
//      handleCoord(input.substring(6).c_str());  
//    } else if (input.equalsIgnoreCase("HOME")) {
//      homePosition();
//    } else if (input.equalsIgnoreCase("TEST")) {
//      testFullDistance();
//    } else if (input.equalsIgnoreCase("WHERE")) {
//      reportCurrentPosition();
//    } else {
//      Serial.println("Unknown command");
//    }
//
//    Serial.println("Done");
//    analogWrite(LED_PIN, 255); 
//  }
//}
//
//bool checkObstacle() {
//  static int confirmCount = 0;
//  
//  bool obstacleDetected = (digitalRead(SENSOR_PIN_1) == LOW || digitalRead(SENSOR_PIN_2) == LOW);
//  
//  if (obstacleDetected) {
//    confirmCount++;
//    if (confirmCount >= OBSTACLE_CONFIRM_COUNT) {
//      confirmCount = OBSTACLE_CONFIRM_COUNT; 
//      return true;
//    }
//  } else {
//    confirmCount = 0;
//  }
//  
//  return false;
//}
//
//void handleCoord(const char* coordData) {
//  String coordStr = String(coordData);
//  coordStr.trim();
//
//  int commaIndex = coordStr.indexOf(',');
//  if (commaIndex != -1) {
//    String xStr = coordStr.substring(0, commaIndex);
//    String yStr = coordStr.substring(commaIndex + 1);
//    xStr.trim();
//    yStr.trim();
//
//    float x = xStr.toFloat();
//    
//    x *= CALIBRATION_FACTOR;
//    
//    moveToPosition(x);
//  } else {
//    Serial.println("Invalid COORD format. Use: COORD:x,y");
//  }
//}
//
//void moveToPosition(float targetX) {
//  long targetPosition = round(targetX * STEPS_PER_MM);
//  
//  if (targetPosition < 0) targetPosition = 0;
//  if (targetPosition > MAX_POSITION_STEPS) targetPosition = MAX_POSITION_STEPS;
//  
//  long stepsToMove = targetPosition - currentPosition;
//  
//  if (stepsToMove == 0) {
//    Serial.println("Already at target position, no movement needed");
//    return;
//  }
//  
//  int stepDirection = (stepsToMove >= 0) ? 1 : -1;
//  digitalWrite(X_DIR_PIN, stepsToMove >= 0 ? HIGH : LOW);
//  
//  long stepCount = abs(stepsToMove);
//  long stepsMoved = 0;
//  
//  for (long i = 0; i < stepCount; i++) {
//    if (checkObstacle()) {
////      Serial.println("EMERGENCY STOP! Movement halted.");
//      digitalWrite(LED_MOTION_PIN, HIGH);
//      currentPosition += stepsMoved * stepDirection; 
////      Serial.print("Stopped at ");
////      Serial.print(currentPosition / STEPS_PER_MM);
////      Serial.println("mm");
//      return;
//    }
//    
//    digitalWrite(X_STEP_PIN, HIGH);
//    delayMicroseconds(10);
//    digitalWrite(X_STEP_PIN, LOW);
//    delayMicroseconds(STEP_PERIOD_US);
//    
//    stepsMoved++;
//  }
//  
//  currentPosition += stepsMoved * stepDirection;
//}
//
//void homePosition() {
//  moveToPosition(0.0);
//  currentPosition = 0; 
//}
//
//void testFullDistance() {
//  moveToPosition(610.0);
//}
//
//void reportCurrentPosition() {
//  Serial.print("Current position: ");
//  Serial.print(currentPosition);
//  Serial.print(" steps (");
//  Serial.print(currentPosition / STEPS_PER_MM);
//  Serial.println(" mm)");
//}
//
//void checkEdgeEvent() {
//  bool nearLeftEdge = (currentPosition <= EDGE_THRESHOLD);
//  bool nearRightEdge = (currentPosition >= (MAX_POSITION_STEPS - EDGE_THRESHOLD));
//  
//  if (nearLeftEdge || nearRightEdge) {
//    unsigned long currentTime = millis();
//    
//    if (currentTime - lastEdgeTime < EDGE_TIMEOUT) {
//      edgePassCount++;
//    } else {
//      edgePassCount = 1;
//    }
//    
//    lastEdgeTime = currentTime;
//
//    if (edgePassCount >= 2) {
//      blinkAlertLED();
//    }
//  } else {
//    edgePassCount = 0;
//  }
//}
//
//void blinkAlertLED() {
//  static unsigned long lastBlinkTime = 0;
//  const unsigned long BLINK_INTERVAL = 500; 
//  
//  unsigned long currentTime = millis();
//  
//  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
//    lastBlinkTime = currentTime;
//    ledBlinkState = !ledBlinkState;
//    analogWrite(LED_PIN, ledBlinkState ? 255 : 0);
//  }
//  
//  if (currentTime - lastEdgeTime > EDGE_TIMEOUT) {
//    edgePassCount = 0;
//    analogWrite(LED_PIN, 255); 
//  }
//}






#include <Arduino.h>

#define X_STEP_PIN     2
#define X_DIR_PIN      5
#define ENABLE_PIN     8
#define LED_PIN        A3
#define V_5            A0
#define LED_MOTION_PIN A1   
#define IR_SENSOR_PIN  11 
#define IR_POWER_PIN   A2  
#define SENSOR_PIN_1   12
#define SENSOR_PIN_2   13

const int STEPS_PER_REV = 3200;
const float PULLEY_DIAMETER = 13.0;       
const float BELT_PITCH = 1.0;              
const float STEPS_PER_MM = STEPS_PER_REV / (PI * PULLEY_DIAMETER);
const float MAX_DISTANCE_MM = 610.0;       
const long MAX_POSITION_STEPS = round(MAX_DISTANCE_MM * STEPS_PER_MM);
const int RPM = 150;                       
const unsigned long STEP_PERIOD_US = 60000000UL / (STEPS_PER_REV * RPM);

const int OBSTACLE_CONFIRM_COUNT = 5; 

long currentPosition = 0;  
const int EDGE_THRESHOLD = 50; 
const unsigned long EDGE_TIMEOUT = 60000; 

unsigned long lastEdgeTime = 0;
int edgePassCount = 0;
bool ledBlinkState = false;

const float CALIBRATION_FACTOR = 1;  

void setup() {
  Serial.begin(9600);
  while (!Serial);  
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_MOTION_PIN, OUTPUT);
  pinMode(IR_POWER_PIN, OUTPUT);    
  pinMode(IR_SENSOR_PIN, INPUT);   
  pinMode(V_5, OUTPUT);
  pinMode(SENSOR_PIN_1, INPUT);
  pinMode(SENSOR_PIN_2, INPUT);
  
  digitalWrite(ENABLE_PIN, LOW); 
  digitalWrite(IR_POWER_PIN, LOW); 
  analogWrite(V_5, 255);
  analogWrite(LED_PIN, 255); 

  Serial.println("System Ready");
}

void loop() {
  if (checkObstacle()) {
    digitalWrite(LED_MOTION_PIN, HIGH);
  } else {
    digitalWrite(LED_MOTION_PIN, LOW);
  }

  checkEdgeEvent();

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  

    if (input.length() == 0) return;

    Serial.print("Received: ");
    analogWrite(LED_PIN, 0); 
    Serial.println(input);

    if (input.startsWith("COORD:")) {
      handleCoord(input.substring(6).c_str());  
    } else if (input.equalsIgnoreCase("HOME")) {
      homePosition();
    } else if (input.equalsIgnoreCase("TEST")) {
      testFullDistance();
    } else if (input.equalsIgnoreCase("WHERE")) {
      reportCurrentPosition();
    } else if (input.equalsIgnoreCase("AUTOHOME")) { 
      autoHome();
    } else {
      Serial.println("Unknown command");
    }

    Serial.println("Done");
    analogWrite(LED_PIN, 255); 
  }
}

void autoHome() {
  digitalWrite(IR_POWER_PIN, HIGH);
  delay(50); // Chờ ổn định cảm biến

  digitalWrite(X_DIR_PIN, LOW);
  
  Serial.println("Homing started...");
  bool homingComplete = false;
  const long maxHomingSteps = MAX_POSITION_STEPS + 10000; 

  for (long i = 0; i < maxHomingSteps; i++) {
    if (digitalRead(IR_SENSOR_PIN) == LOW) {
      homingComplete = true;
      break;
    }

    if (checkObstacle()) {
      Serial.println("Obstacle detected during homing!");
      break;
    }

    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(STEP_PERIOD_US);
  }

  if (homingComplete) {
    currentPosition = 0; 
    Serial.println("Homing complete. Position reset to 0.");
  } else {
    Serial.println("Homing failed!");
  }

  digitalWrite(IR_POWER_PIN, LOW);
}

bool checkObstacle() {
  static int confirmCount = 0;
  
  bool obstacleDetected = (digitalRead(SENSOR_PIN_1) == LOW || digitalRead(SENSOR_PIN_2) == LOW);
  
  if (obstacleDetected) {
    confirmCount++;
    if (confirmCount >= OBSTACLE_CONFIRM_COUNT) {
      confirmCount = OBSTACLE_CONFIRM_COUNT; 
      return true;
    }
  } else {
    confirmCount = 0;
  }
  
  return false;
}

void handleCoord(const char* coordData) {
  String coordStr = String(coordData);
  coordStr.trim();

  int commaIndex = coordStr.indexOf(',');
  if (commaIndex != -1) {
    String xStr = coordStr.substring(0, commaIndex);
    String yStr = coordStr.substring(commaIndex + 1);
    xStr.trim();
    yStr.trim();

    float x = xStr.toFloat();
    
    x *= CALIBRATION_FACTOR;
    
    moveToPosition(x);
  } else {
    Serial.println("Invalid COORD format. Use: COORD:x,y");
  }
}

void moveToPosition(float targetX) {
  long targetPosition = round(targetX * STEPS_PER_MM);
  
  if (targetPosition < 0) targetPosition = 0;
  if (targetPosition > MAX_POSITION_STEPS) targetPosition = MAX_POSITION_STEPS;
  
  long stepsToMove = targetPosition - currentPosition;
  
  if (stepsToMove == 0) {
    Serial.println("Already at target position, no movement needed");
    return;
  }
  
  int stepDirection = (stepsToMove >= 0) ? 1 : -1;
  digitalWrite(X_DIR_PIN, stepsToMove >= 0 ? HIGH : LOW);
  
  long stepCount = abs(stepsToMove);
  long stepsMoved = 0;
  
  for (long i = 0; i < stepCount; i++) {
    if (checkObstacle()) {
      digitalWrite(LED_MOTION_PIN, HIGH);
      currentPosition += stepsMoved * stepDirection; 
      return;
    }
    
    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(STEP_PERIOD_US);
    
    stepsMoved++;
  }
  
  currentPosition += stepsMoved * stepDirection;
}

void homePosition() {
  moveToPosition(0.0);
  currentPosition = 0; 
}

void testFullDistance() {
  moveToPosition(610.0);
}

void reportCurrentPosition() {
  Serial.print("Current position: ");
  Serial.print(currentPosition);
  Serial.print(" steps (");
  Serial.print(currentPosition / STEPS_PER_MM);
  Serial.println(" mm)");
}

void checkEdgeEvent() {
  bool nearLeftEdge = (currentPosition <= EDGE_THRESHOLD);
  bool nearRightEdge = (currentPosition >= (MAX_POSITION_STEPS - EDGE_THRESHOLD));
  
  if (nearLeftEdge || nearRightEdge) {
    unsigned long currentTime = millis();
    
    if (currentTime - lastEdgeTime < EDGE_TIMEOUT) {
      edgePassCount++;
    } else {
      edgePassCount = 1;
    }
    
    lastEdgeTime = currentTime;

    if (edgePassCount >= 2) {
      blinkAlertLED();
    }
  } else {
    edgePassCount = 0;
  }
}

void blinkAlertLED() {
  static unsigned long lastBlinkTime = 0;
  const unsigned long BLINK_INTERVAL = 500; 
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = currentTime;
    ledBlinkState = !ledBlinkState;
    analogWrite(LED_PIN, ledBlinkState ? 255 : 0);
  }
  
  if (currentTime - lastEdgeTime > EDGE_TIMEOUT) {
    edgePassCount = 0;
    analogWrite(LED_PIN, 255); 
  }
}
