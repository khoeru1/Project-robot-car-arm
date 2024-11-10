#include <PS3Controller.h>
#include <ESP32Servo.h>

// Inisialisasi servo
Servo base;     
Servo shoulder; 
Servo elbow;    
Servo gripper;  

// Pin untuk servo
#define BASE_PIN 13
#define SHOULDER_PIN 12 
#define ELBOW_PIN 14
#define GRIPPER_PIN 27

// Pin untuk motor DC
#define MOTOR_PIN1 25
#define MOTOR_PIN2 26
#define MOTOR_PIN3 32
#define MOTOR_PIN4 33

// PWM Channels
#define PWM_CHANNEL_0 0
#define PWM_CHANNEL_1 1
#define PWM_CHANNEL_2 2
#define PWM_CHANNEL_3 3

// PWM Properties
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

// Posisi awal servo
int basePos = 90;     
int shoulderPos = 90; 
int elbowPos = 90;    
int gripperPos = 90;  

// Variable motor speed
int motorSpeed = 200;

void notify()
{
  // Analog stick kiri untuk menggerakkan robot
  int yAxis = (PS3.data.analog.stick.ly); // Maju mundur
  int xAxis = (PS3.data.analog.stick.lx); // Kiri kanan
  
  // Motor control berdasarkan analog stick
  if (yAxis > 20) { // Maju
    ledcWrite(PWM_CHANNEL_0, motorSpeed);
    ledcWrite(PWM_CHANNEL_1, 0);
    ledcWrite(PWM_CHANNEL_2, motorSpeed);
    ledcWrite(PWM_CHANNEL_3, 0);
  }
  else if (yAxis < -20) { // Mundur
    ledcWrite(PWM_CHANNEL_0, 0);
    ledcWrite(PWM_CHANNEL_1, motorSpeed);
    ledcWrite(PWM_CHANNEL_2, 0);
    ledcWrite(PWM_CHANNEL_3, motorSpeed);
  }
  else if (xAxis < -20) { // Kiri
    ledcWrite(PWM_CHANNEL_0, 0);
    ledcWrite(PWM_CHANNEL_1, motorSpeed);
    ledcWrite(PWM_CHANNEL_2, motorSpeed);
    ledcWrite(PWM_CHANNEL_3, 0);
  }
  else if (xAxis > 20) { // Kanan
    ledcWrite(PWM_CHANNEL_0, motorSpeed);
    ledcWrite(PWM_CHANNEL_1, 0);
    ledcWrite(PWM_CHANNEL_2, 0);
    ledcWrite(PWM_CHANNEL_3, motorSpeed);
  }
  else { // Berhenti
    ledcWrite(PWM_CHANNEL_0, 0);
    ledcWrite(PWM_CHANNEL_1, 0);
    ledcWrite(PWM_CHANNEL_2, 0);
    ledcWrite(PWM_CHANNEL_3, 0);
  }

  // Analog stick kanan untuk kontrol arm
  // Base control (kiri-kanan)
  if (PS3.data.analog.stick.rx > 20) {
    basePos++;
    if (basePos > 180) basePos = 180;
    base.write(basePos);
  }
  if (PS3.data.analog.stick.rx < -20) {
    basePos--;
    if (basePos < 0) basePos = 0;
    base.write(basePos);
  }

  // Shoulder control (atas-bawah)
  if (PS3.data.analog.stick.ry < -20) {
    shoulderPos++;
    if (shoulderPos > 180) shoulderPos = 180;
    shoulder.write(shoulderPos);
  }
  if (PS3.data.analog.stick.ry > 20) {
    shoulderPos--;
    if (shoulderPos < 0) shoulderPos = 0;
    shoulder.write(shoulderPos);
  }

  // Elbow control dengan R1 dan L1
  if (PS3.data.button.r1) {
    elbowPos++;
    if (elbowPos > 180) elbowPos = 180;
    elbow.write(elbowPos);
  }
  if (PS3.data.button.l1) {
    elbowPos--;
    if (elbowPos < 0) elbowPos = 0;
    elbow.write(elbowPos);
  }

  // Gripper control dengan R2 dan L2
  if (PS3.data.button.r2) {
    gripperPos++;
    if (gripperPos > 180) gripperPos = 180;
    gripper.write(gripperPos);
  }
  if (PS3.data.button.l2) {
    gripperPos--;
    if (gripperPos < 0) gripperPos = 0;
    gripper.write(gripperPos);
  }
}

void onConnect() {
  Serial.println("Connected.");
}

void setup()
{
  Serial.begin(115200);

  // Setup PWM untuk motor DC
  ledcSetup(PWM_CHANNEL_0, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_2, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_3, PWM_FREQ, PWM_RESOLUTION);

  // Attach PWM channel ke GPIO pins
  ledcAttachPin(MOTOR_PIN1, PWM_CHANNEL_0);
  ledcAttachPin(MOTOR_PIN2, PWM_CHANNEL_1);
  ledcAttachPin(MOTOR_PIN3, PWM_CHANNEL_2);
  ledcAttachPin(MOTOR_PIN4, PWM_CHANNEL_3);

  // Initialize ESP32Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Attach servo
  base.setPeriodHertz(50);
  shoulder.setPeriodHertz(50);
  elbow.setPeriodHertz(50);
  gripper.setPeriodHertz(50);
  
  base.attach(BASE_PIN);
  shoulder.attach(SHOULDER_PIN);
  elbow.attach(ELBOW_PIN);
  gripper.attach(GRIPPER_PIN);

  // Set posisi awal
  base.write(basePos);
  shoulder.write(shoulderPos);
  elbow.write(elbowPos);
  gripper.write(gripperPos);

  // Initialize PS3 Controller
  PS3.attach(notify);
  PS3.attachOnConnect(onConnect);
  PS3.begin("01:02:03:04:05:06"); // Ganti dengan MAC Address ESP32 Anda

  Serial.println("Ready.");
}

void loop()
{
  // Loop utama kosong karena menggunakan callback
  delay(1);
}