#include <WiFiNINA.h>
#include <ThingSpeak.h>

char ssid[] = "HHY0812";    // WiFi网络名称
char pass[] = "20010812";  // WiFi网络密码

const int trigPin = 2;  // Trig引脚连接到Arduino的2号引脚
const int echoPin = 3;  // Echo引脚连接到Arduino的3号引脚
const int ledPin = 8;   // 控制小灯的引脚连接到Arduino的8号引脚

const char *thingSpeakAddress = "api.thingspeak.com";
const String writeAPIKey = "G8KKOD51LSZ29BT8";   // Write API Key
const int channelID = 2398901;          //  Channel ID

WiFiClient client;

void setup() {
  Serial.begin(9600);

  // WiFi连接
  connectToWiFi();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // 初始化 ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  long duration, distance;

  // 发送超声波脉冲
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 读取超声波传感器返回的脉冲宽度
  duration = pulseIn(echoPin, HIGH);

  // 将脉冲宽度转换为距离（单位：厘米）
  distance = duration * 0.034 / 2;

  // 在串口监视器上打印测量距离
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // 根据距离控制小灯
  if (distance < 50) {
    digitalWrite(ledPin, HIGH);  // 距离小于50cm时，点亮小灯
  } else {
    digitalWrite(ledPin, LOW);   // 距离大于等于50cm时，熄灭小灯
  }

  // 将数据上传到 ThingSpeak
  uploadToThingSpeak(distance, digitalRead(ledPin));

  delay(1000);  // 等待1秒后进行下一次测量
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void uploadToThingSpeak(int distance, int ledState) {
  ThingSpeak.setField(1, distance);
  ThingSpeak.setField(2, ledState);

  int status = ThingSpeak.writeFields(channelID, writeAPIKey.c_str());
  if (status == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Failed to connect to ThingSpeak");
  }

  delay(2000);  // 等待2秒后进行下一次上传
}
