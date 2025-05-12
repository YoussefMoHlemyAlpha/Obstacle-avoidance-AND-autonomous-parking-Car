#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <NewPing.h>
#include <ESP32Servo.h>

// WiFi credentials (hotspot)
const char* ssid = "MT";
const char* password = "232003#r";

// HiveMQ Cloud credentials ( mqtt info)
const char* mqtt_server = "19336e390ca443c6a04e2ddad325c45c.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "maro23";
const char* mqtt_password = "4vr]4}GqdwSF+n]";
const char* client_id = "ESP32Client";

// MQTT topics
const char* publish_topic = "esp32/output";
const char* subscribe_topic = "esp32/input";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Motor pins
#define IN1 22
#define IN2 21
#define IN3 19
#define IN4 18
#define ENA 5
#define ENB 23

#define MAX_SPEED 200

// Ultrasonic pins
#define TRIG1 14
#define ECHO1 15
#define TRIG2 26
#define ECHO2 25
#define TRIG3 13
#define ECHO3 2
#define MAX_DISTANCE 200



NewPing sonar(TRIG3, ECHO3, MAX_DISTANCE);


// Fixed: Declare command as a global variable and initialize it
char command = 's';
int distance = 100;
double space = 0;
bool goesForward = false;

// Setup WiFi and MQTT connection
void setup_wifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(subscribe_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// Callback to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println("Payload: " + msg);

  // Fixed: Actually update the command variable based on received message
  if (msg == "o") {
    Serial.println("Switched to obstacle detection mode");
    command = 'o';
  } else if (msg == "p") {
    Serial.println("Switched to autonomous parking mode");
    command = 'p';
  } else if (msg == "s") {
    Serial.println("Engines Stopped");
    command = 's';
  }

  

}

void setup() {
  Serial.begin(115200);

  // Set insecure TLS (skip validation — for testing only!)
  espClient.setInsecure();

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);



}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // Debug output to monitor current command
  static char lastCommand = ' ';
  if (lastCommand != command) {
    String debugMsg = "Executing command: ";
    debugMsg += command;
    Serial.println(debugMsg);
    lastCommand = command;
  }

  switch (command) {
    case 's': moveStop(); break;
    case 'o': obstacleAvoidance(); break;
    case 'p':
      if (check() == 1 && space > 5) {
        space = 0;
        moveStop();
        delay(1000);
        park();
        exit(0);

      } else {
        if (UFun(TRIG1 , ECHO1) > 35 && UFun(TRIG3, ECHO3) > 35) {
          moveForward();
        } else {
          moveStop();
        }
        break;
        default:
          moveStop();
          break;
      }

      delay(100);
  }
}
// Ultrasonic reading
int UFun(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 20000);  // timeout 20ms
  int distance = duration * 0.034 / 2;
  return distance;
}

void moveStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  goesForward = false;
}

void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, MAX_SPEED);
  analogWrite(ENB, MAX_SPEED);
  goesForward = true;
}

void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, MAX_SPEED);
  analogWrite(ENB, MAX_SPEED);
  goesForward = false;
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, MAX_SPEED);
  analogWrite(ENB, MAX_SPEED);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, MAX_SPEED);
  analogWrite(ENB, MAX_SPEED);
}

void obstacleAvoidance() {
  int frontDist = UFun(TRIG3, ECHO3);
  Serial.println(frontDist);
  if (frontDist > 20) {
    moveForward();
  } else {
    moveStop();
    delay(300);
    turnRight();
    delay(400);
    moveForward();
  }
}

int check() {  //Chicking the sutable parking space for the car width
  int Flag = 0;
  while (UFun(TRIG2, ECHO2) > 14) {
    moveForward();
    space++;
    Flag = 1;
  }
  return Flag;
}

void park() {
  // moveBackward();
  // delay(800);
  moveStop();
  delay(1000);
  turnLeft();
  delay(600);
  moveStop();
  while (UFun(TRIG1, ECHO1) > 12) {
    moveBackward();
    delay(50);
  }
  moveStop();
  delay(500);
  turnRight();
  delay(700);
  moveStop();
}