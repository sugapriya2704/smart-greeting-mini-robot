#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "ESP32_ROBOT";
const char* password = "12345678";

WebServer server(80);

// Motor pins
#define IN1 26
#define IN2 27
#define IN3 14
#define IN4 12
#define ENA 25
#define ENB 33

// PWM settings
#define PWM_FREQ 1000
#define PWM_RESOLUTION 8
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1

int speedValue = 150;

// ===== SERVOS =====
Servo myServo;          // Center servo
Servo leftServo;        // Left hand
Servo rightServo;       // Right hand

#define SERVO_PIN 4
#define LEFT_SERVO_PIN 16
#define RIGHT_SERVO_PIN 17

// ==========================================================

void setup() {

  Serial.begin(115200);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Motor PWM
  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNEL_A);

  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENB, PWM_CHANNEL_B);

  ledcWrite(PWM_CHANNEL_A, speedValue);
  ledcWrite(PWM_CHANNEL_B, speedValue);

  // ===== Servo Timers =====
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Center Servo
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(0);

  // Left Servo
  leftServo.setPeriodHertz(50);
  leftServo.attach(LEFT_SERVO_PIN, 500, 2400);
  leftServo.write(0);

  // Right Servo
  rightServo.setPeriodHertz(50);
  rightServo.attach(RIGHT_SERVO_PIN, 500, 2400);
  rightServo.write(0);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/forward", moveForward);
  server.on("/reverse", moveReverse);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/stop", stopRobot);
  server.on("/speed", setSpeed);

  // Servo APIs
  server.on("/up", servoUp);
  server.on("/down", servoDown);

  server.on("/leftup", leftUp);
  server.on("/leftdown", leftDown);

  server.on("/rightup", rightUp);
  server.on("/rightdown", rightDown);

  server.begin();
}

void loop() {
  server.handleClient();
}

// ================= SPEED =================
void setSpeed() {
  if (server.hasArg("value")) {
    speedValue = server.arg("value").toInt();
    ledcWrite(PWM_CHANNEL_A, speedValue);
    ledcWrite(PWM_CHANNEL_B, speedValue);
  }
  server.send(200, "text/plain", "");
}

// ================= MOTOR =================
void moveForward() {
  ledcWrite(PWM_CHANNEL_A, speedValue);
  ledcWrite(PWM_CHANNEL_B, speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  server.send(200, "text/plain", "");
}

void moveReverse() {
  ledcWrite(PWM_CHANNEL_A, speedValue);
  ledcWrite(PWM_CHANNEL_B, speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  server.send(200, "text/plain", "");
}

void turnLeft() {
  ledcWrite(PWM_CHANNEL_A, speedValue);
  ledcWrite(PWM_CHANNEL_B, speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  server.send(200, "text/plain", "");
}

void turnRight() {
  ledcWrite(PWM_CHANNEL_A, speedValue);
  ledcWrite(PWM_CHANNEL_B, speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  server.send(200, "text/plain", "");
}

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(PWM_CHANNEL_A, 0);
  ledcWrite(PWM_CHANNEL_B, 0);
  server.send(200, "text/plain", "");
}

// ================= CENTER SERVO =================
void servoUp() {
  myServo.write(90);
  server.send(200, "text/plain", "");
}

void servoDown() {
  myServo.write(0);
  server.send(200, "text/plain", "");
}

// ================= LEFT HAND =================
void leftUp() {
  leftServo.write(90);
  server.send(200, "text/plain", "");
}

void leftDown() {
  leftServo.write(0);
  server.send(200, "text/plain", "");
}

// ================= RIGHT HAND =================
void rightUp() {
  rightServo.write(90);
  server.send(200, "text/plain", "");
}

void rightDown() {
  rightServo.write(0);
  server.send(200, "text/plain", "");
}

// ================= WEB PAGE =================
void handleRoot() {

String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{background:#111;text-align:center;font-family:Arial;color:white;}
button{width:120px;height:60px;margin:8px;font-size:16px;border:none;border-radius:10px;background:#00cc77;}
.stop{background:red;}
</style>
</head>
<body>

<h2>ROBOT CONTROL</h2>

<button onclick="fetch('/forward')">FORWARD</button><br>
<button onclick="fetch('/left')">LEFT</button>
<button onclick="fetch('/right')">RIGHT</button><br>
<button onclick="fetch('/reverse')">REVERSE</button><br>
<button class="stop" onclick="fetch('/stop')">STOP</button>

<h3>CENTER SERVO</h3>
<button onclick="fetch('/up')">UP</button>
<button onclick="fetch('/down')">DOWN</button>

<h3>LEFT HAND</h3>
<button onclick="fetch('/leftup')">UP</button>
<button onclick="fetch('/leftdown')">DOWN</button>

<h3>RIGHT HAND</h3>
<button onclick="fetch('/rightup')">UP</button>
<button onclick="fetch('/rightdown')">DOWN</button>

<br><br>
<input type="range" min="0" max="255" value="150"
oninput="fetch('/speed?value='+this.value)">

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}
