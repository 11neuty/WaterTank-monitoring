#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>       
#include <SoftwareSerial.h>

#define USE_SSL true

#define FIREBASE_HOST "liquid-tank-e35c2-default-rtdb.firebaseio.com"      
#define FIREBASE_AUTH "934eb36aa6ddf6da2b3550b463f89a69abfa31d5"  

FirebaseData firebaseData;
FirebaseJson json;
const char* ssid = "ssid";
const char* pass = "pass";
WiFiClient wifiClient;

static const int trigPin = 2, echoPin = 0;
const int phi = 3.14;
const int r = 35;
int liter,tinggi;
float vol;
float duration, distance;

void post_to_firebase(String data);
boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000);

void connectWiFi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (pass && strlen(pass)) {
    WiFi.begin((char*)ssid, (char*)pass);
  } else {
    WiFi.begin((char*)ssid);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
void loop() {
  sensor(500);
  volume();
  displaydata();
  
  // Reconnect WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    return;
  }

  
  }

void sensor(unsigned long ms)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  //distance = jarak ketinggian air dalam tangki--
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
}
void volume(){
  tinggi = 104 - distance;
  vol = phi*r*r*tinggi;
  liter = vol/1000;
  
  Serial.print("Volume air =");
  Serial.println(vol);
  Serial.println("kubik");
  delay(500);
  Serial.print("Liter air = ");
  Serial.println(liter);
  delay(500);
  Serial.print("Tinggi Air = ");
  Serial.println(tinggi);
}

void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}

void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}

void displaydata()
{
  if(Firebase.setFloat(firebaseData, "/LIQUID-TANK/kubik", vol))
    {
    print_ok();}
   if(Firebase.setInt(firebaseData, "/LIQUID-TANK/liter", liter))
      {
        print_ok();
      }
    if(Firebase.setInt(firebaseData, "/LIQUID-TANK/tinggi", tinggi))
      {
        print_ok();
      }
    
    
    else
      {
      print_fail();
      }
  delay(1000);
}
