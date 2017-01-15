#include <Servo.h>
#include <WiFi.h>

const char* ssid = "your ssid";
const char* password = "your password";

//ClientDefine
const String ENGINE_START = "1";
const String ENGINE_BACK = "2";
const String ENGINE_STOP = "3";
const String SHIFT_HI = "4";
const String SHIFT_NEUTRAL ="5";
const String SHIFT_LOW = "6";
const String STEERING_RIGHT = "7";
const String STEERING_NEUTRAL = "8";
const String STEERING_LEFT = "9";

//Servo motor pin
const int SERVO_SHIFT_PIN = 3;
const int SERVO_STEERING_PIN = 6;
const int SERVO_ENGINE_PIN = 9;

//Servo motor for the shift
const int SERVO_SHIFT_NEUTRAL = 90;
const int SERVO_SHIFT_HI = 75;
const int SERVO_SHIFT_HI_TO_NEUTRAL = 105;
const int SERVO_SHIFT_LOW = 135;
const int SERVO_SHIFT_LOW_TO_NEUTRAL = 100;

//Servo motor for the steering
const int SERVO_STEERING_NEUTRAL = 90;
const int SERVO_STEERING_RIGHT = 130;
const int SERVO_STEERING_LEFT = 50;

//Servo motor for the engine
const int SERVO_ENGINE_STOP = 90;
const int SERVO_ENGINE_START = 65;
const int SERVO_ENGINE_START_TO_STOP = 95;
const int SERVO_ENGINE_BACK_TO_STOP = 75;
const int SERVO_ENGINE_BACK = 105;

Servo shift_servo;
Servo engine_servo;
Servo steering_servo;

WiFiServer server(8000);
WiFiClient client;

void connectWiFi(const char* ssid ,const char* password)
{
  WiFi.disconnect();
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid ,password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // put your setup code here, to run once:
  connectWiFi(ssid ,password);
  server.begin();
  client = server.available();
}

void loop() {
  // put your main code here, to run repeatedly:
  while ((WiFi.status() != WL_CONNECTED)) {
    connectWiFi(ssid ,password);
  }

  while (!client) {
    client = server.available();
    delay(1);
  }
  
  while (!client.available()) {
    delay(1);
  }
  Serial.println("client connected");

  String res = client.readStringUntil('\n');
  Serial.println(res);
  client.flush();
  client.stop();
  int ret = 0;

  // 以降はClientから操作を順次チェック
  ret = checkEngine(res);
  if (0 != ret) {
    engineServoMove(ret);
    delay(700);
    return;
  } 

  ret = checkShift(res);
  if (0 != ret) {
    shiftServoMove(ret);
    delay(700);
    return;
  } 
  
  ret = checkSteering(res);
  if (0 != ret) {
    steeringServoMove(ret);
    delay(700);
    return;
  } 
  
  delay(1000);
}

/*
 * エンジンに対する命令かをチェック
 */
int checkEngine (String res)
{
  int ret = 0;
  if (res.equals(ENGINE_START)) {
    ret = SERVO_ENGINE_START;
  } else if (res.equals(ENGINE_BACK)) {
    ret = SERVO_ENGINE_BACK;
  } else if (res.equals(ENGINE_STOP)) {
    ret = SERVO_ENGINE_STOP;
  }
  return ret;
}

/*
 * シフトに対する命令かをチェック
 */
int checkShift (String res)
{
  int ret = 0;
  if (res.equals(SHIFT_HI)) {
    ret = SERVO_SHIFT_HI;
  } else if (res.equals(SHIFT_NEUTRAL)) {
    ret = SERVO_SHIFT_NEUTRAL;
  } else if (res.equals(SHIFT_LOW)) {
    ret = SERVO_SHIFT_LOW;
  }
  return ret;
}

/*
 * ステアリングに対する命令かをチェック
 */
int checkSteering (String res)
{
  int ret = 0;
  if (res.equals(STEERING_RIGHT)) {
    ret = SERVO_STEERING_RIGHT;
  } else if (res.equals(STEERING_NEUTRAL)) {
    ret = SERVO_STEERING_NEUTRAL;
  } else if (res.equals(STEERING_LEFT)) {
    ret = SERVO_STEERING_LEFT;
  }
  return ret;
}

/*
 * エンジンのサーボを動かす
 */
void engineServoMove(int angle)
{
  // 他のサーボモータは念のためデタッチ
  steering_servo.detach();
  shift_servo.detach();
  if (SERVO_ENGINE_STOP == angle) {
    switch (engine_servo.read())
    {
      case SERVO_ENGINE_START:
        angle = SERVO_ENGINE_START_TO_STOP;
        break;
      case SERVO_ENGINE_BACK:
        angle = SERVO_ENGINE_BACK_TO_STOP;
        break;
    }
  }
  engine_servo.attach(SERVO_ENGINE_PIN);
  engine_servo.write(angle);
  delay(300);
  engine_servo.detach();
}

/*
 * シフトのサーボを動かす
 */
void shiftServoMove(int angle)
{
  // 他のサーボモータは念のためデタッチ
  engine_servo.detach();
  steering_servo.detach();
  if (SERVO_SHIFT_NEUTRAL == angle) {
    switch (shift_servo.read())
    {
      case SERVO_SHIFT_HI:
        angle = SERVO_SHIFT_HI_TO_NEUTRAL;
        break;
      case SERVO_SHIFT_LOW:
        angle = SERVO_SHIFT_LOW_TO_NEUTRAL;
        break;
    }
  }
  shift_servo.attach(SERVO_SHIFT_PIN);
  shift_servo.write(angle);
  delay(300);
  shift_servo.detach();
}

/*
 * ステアリングのサーボを動かす
 */
void steeringServoMove(int angle)
{
  // 他のサーボモータは念のためデタッチ
  engine_servo.detach();
  shift_servo.detach();
  steering_servo.attach(SERVO_STEERING_PIN);
  
  steering_servo.write(angle);
  delay(300);
  steering_servo.detach();
}
