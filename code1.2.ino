#include <AFMotor.h>
#include <Servo.h>

// Motor definitions
AF_DCMotor rightBack(1);
AF_DCMotor rightFront(2);
AF_DCMotor leftFront(3);
AF_DCMotor leftBack(4);
Servo servoLook;

// Sensor pins
const byte trig = A0;
const byte echo = A1;

// Parameters
const byte stopDist = 30;      // Detection distance (30cm)
const byte maxDist = 150;      // Max sensing range
const byte motorSpeed = 100;   // Base speed (0-255)
const int reverseTime = 800;   // Reverse duration (ms)
const int rotateTime = 400;    // Rotation duration (ms)

void setup() {
  Serial.begin(9600);
  
  // Initialize motors
  rightBack.setSpeed(motorSpeed);
  rightFront.setSpeed(motorSpeed);
  leftFront.setSpeed(motorSpeed);
  leftBack.setSpeed(motorSpeed);
  stopMotors();
  
  // Attach servo to pin 10
  servoLook.attach(10);
  
  // Ultrasonic setup
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  int distance = getDistance();
  
  if (distance <= stopDist) {
    stopMotors();
    Serial.println("Obstacle detected!");
    
    // Reverse first
    reverse();
    delay(reverseTime);
    stopMotors();
    
    // Check directions
    int direction = checkDirection();
    
    // Rotate in place based on clearer path
    if (direction == 0) {
      rotateLeft(rotateTime);
    } else {
      rotateRight(rotateTime);
    }
    
    delay(500);
  } else {
    moveForward();
  }
}

//---------------- Helper Functions ----------------//

int checkDirection() {
  int leftDistance = 0, rightDistance = 0;
  
  // Look left
  servoLook.write(180);
  delay(500);
  leftDistance = getDistance();
  
  // Look right
  servoLook.write(0);
  delay(500);
  rightDistance = getDistance();
  
  // Reset servo
  servoLook.write(90);
  delay(500);
  
  Serial.print("Left: ");
  Serial.print(leftDistance);
  Serial.print("cm | Right: ");
  Serial.print(rightDistance);
  Serial.println("cm");
  
  return (leftDistance > rightDistance) ? 0 : 1; // 0=left, 1=right
}

void moveForward() {
  rightBack.run(FORWARD);
  rightFront.run(FORWARD);
  leftFront.run(FORWARD);
  leftBack.run(FORWARD);
}

void reverse() {
  rightBack.run(BACKWARD);
  rightFront.run(BACKWARD);
  leftFront.run(BACKWARD);
  leftBack.run(BACKWARD);
}

void rotateLeft(int duration) {
  rightBack.run(FORWARD);
  rightFront.run(FORWARD);
  leftFront.run(BACKWARD);
  leftBack.run(BACKWARD);
  delay(duration);
  stopMotors();
}

void rotateRight(int duration) {
  rightBack.run(BACKWARD);
  rightFront.run(BACKWARD);
  leftFront.run(FORWARD);
  leftBack.run(FORWARD);
  delay(duration);
  stopMotors();
}

void stopMotors() {
  rightBack.run(RELEASE);
  rightFront.run(RELEASE);
  leftFront.run(RELEASE);
  leftBack.run(RELEASE);
}

int getDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  long duration = pulseIn(echo, HIGH, 30000); // Timeout for 30cm max
  int distance = duration * 0.034 / 2;        // Convert to cm
  return (distance <= 0) ? maxDist : distance;
}