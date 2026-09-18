#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char* ssid = "ESP8266_ROBOT";
const char* password = "12345678";

ESP8266WebServer server(80);

// Motor pins (NodeMCU)
#define IN1 5   // D1
#define IN2 4   // D2
#define IN3 14  // D5
#define IN4 12  // D6
#define ENA 13  // D7
#define ENB 15  // D8

// ===== LED PIN =====
#define LED_PIN 10   // SD3 (Safe GPIO for LED control)

int speedValue = 600;  // 0–1023 for ESP8266 PWM

// ===== SERVOS =====
Servo myServo;
Servo leftServo;
Servo rightServo;

#define SERVO_PIN 2       // D4
#define LEFT_SERVO_PIN 0  // D3
#define RIGHT_SERVO_PIN 16 // D0

// ==========================================================

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // LED Setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  leftServo.attach(LEFT_SERVO_PIN);
  leftServo.write(0);

  rightServo.attach(RIGHT_SERVO_PIN);
  rightServo.write(0);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/forward", moveForward);
  server.on("/reverse", moveReverse);
  server.on("/left", turnLeft);
  server.on("/right", turnRight);
  server.on("/stop", stopRobot);
  server.on("/speed", setSpeed);

  server.on("/up", servoUp);
  server.on("/down", servoDown);
  server.on("/leftup", leftUp);
  server.on("/leftdown", leftDown);
  server.on("/rightup", rightUp);
  server.on("/rightdown", rightDown);

  // ===== LED APIs =====
  server.on("/ledon", ledOn);
  server.on("/ledoff", ledOff);

  server.begin();
}

void loop() {
  server.handleClient();
}

// ================= LED CONTROL =================
void ledOn() {
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/plain", "");
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "");
}

// ================= SPEED =================
void setSpeed() {
  if (server.hasArg("value")) {
    speedValue = server.arg("value").toInt();
    analogWrite(ENA, speedValue);
    analogWrite(ENB, speedValue);
  }
  server.send(200, "text/plain", "");
}

// ================= MOTOR =================
void moveForward() {
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  server.send(200, "text/plain", "");
}

void moveReverse() {
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  server.send(200, "text/plain", "");
}

void turnLeft() {
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  server.send(200, "text/plain", "");
}

void turnRight() {
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
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
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  server.send(200, "text/plain", "");
}

// ================= SERVOS =================
void servoUp() { myServo.write(90); server.send(200, "text/plain", ""); }
void servoDown() { myServo.write(0); server.send(200, "text/plain", ""); }

void leftUp() { leftServo.write(90); server.send(200, "text/plain", ""); }
void leftDown() { leftServo.write(0); server.send(200, "text/plain", ""); }

void rightUp() { rightServo.write(90); server.send(200, "text/plain", ""); }
void rightDown() { rightServo.write(0); server.send(200, "text/plain", ""); }

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

<h3>LED CONTROL</h3>
<button onclick="fetch('/ledon')">LED ON</button>
<button onclick="fetch('/ledoff')">LED OFF</button>

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
<input type="range" min="0" max="1023" value="600"
oninput="fetch('/speed?value='+this.value)">

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}
