#include <esp_now.h>
#include <WiFi.h>

unsigned long lastRecvTime = 0;

// Right motor
int enableRightMotor = 22; 
int rightMotorPin1 = 16;
int rightMotorPin2 = 17;

// Left motor
int enableLeftMotor = 23;
int leftMotorPin1 = 18;
int leftMotorPin2 = 19;

// PWM setup
const int PWMFreq = 1000;
const int PWMResolution = 8;

void setupPWM()
{
  ledcAttach(enableRightMotor, PWMFreq, PWMResolution);
  ledcAttach(enableLeftMotor, PWMFreq, PWMResolution);
}

#define MAX_MOTOR_SPEED 200  // keep slightly lower for stability


typedef struct struct_message {
  int lr;
  int ud;
} struct_message;

struct_message incomingData={1930,1874};

  void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t *incomingDataBytes, int len) {
    memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  lastRecvTime = millis(); 
}

void stopMotors()
{
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);
  ledcWrite(enableRightMotor, 0);
  ledcWrite(enableLeftMotor, 0);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  stopMotors();
  setupPWM();
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_11dBm);
    if (esp_now_init() != ESP_OK) return;
    esp_now_register_recv_cb(OnDataRecv);
    lastRecvTime = millis();
  
 
}

void loop() {
  // --- FAILSAFE: Check for lost connection ---
  // If it has been more than 500ms since we last received a packet, STOP.
  if (millis() - lastRecvTime > 500) {
      stopMotors();
      return; // This forces the loop to restart, skipping the movement math below
  }
  
  int y_power = 0; 
  int x_power = 0;

  // calulate ofrward back speed
  if (incomingData.ud > 2000) {
      y_power = map(incomingData.ud, 2000, 4095, 80, 255);
  } else if (incomingData.ud < 1700) {
      y_power = map(incomingData.ud, 1700, 550, 80, -255); // Notice the negative -255!
  }

  //calculate left right speed
  if (incomingData.lr > 2000) {
      // Pushing Left
      x_power = map(incomingData.lr, 2000, 4000, 80, -255);
  } else if (incomingData.lr < 1700) {
      // Pushing Right
      x_power = map(incomingData.lr, 1700, 0, 80, 255);
  }

  // motor mixing
  int left_speed = -y_power - x_power;
  int right_speed = -y_power + x_power;

  // setting motor direction
  if (left_speed >= 0) {
      digitalWrite(leftMotorPin1, HIGH);
      digitalWrite(leftMotorPin2, LOW);
  } else {
      digitalWrite(leftMotorPin1, LOW);
      digitalWrite(leftMotorPin2, HIGH);
  }

  if (right_speed >= 0) {
      digitalWrite(rightMotorPin1, HIGH);
      digitalWrite(rightMotorPin2, LOW);
  } else {
      digitalWrite(rightMotorPin1, LOW);
      digitalWrite(rightMotorPin2, HIGH);
  }

  // cleaning up speed values
  left_speed = constrain(abs(left_speed), 0, 255);
  right_speed = constrain(abs(right_speed), 0, 255);

  // send final speed to motor
  ledcWrite(enableLeftMotor, left_speed);
  ledcWrite(enableRightMotor, right_speed);
  delay(20); // Quick delay for smooth response
}
