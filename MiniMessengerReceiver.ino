// ========================================================================================
//      Meine Welt in meinem Kopf
// ========================================================================================
// Projekt:       Mini Messenger
// Author:        Johannes P. Langner
// Controller:    WEMOS D1 Mini
// Actor:         WS2812b Shield
// Description:   Messenger for get only message.
// ========================================================================================

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <HttpClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>         


// ========================================================================================
// Setup device number
String DEVICE_NUMBER = "1";

// ========================================================================================
// WLAN
const char* mSsid = "ButzBox";
const char* mPassword = "05-nov-1982";

IPAddress mIp(192, 168, 178, 152);             // where xx is the desired IP Address
IPAddress mGateway(192, 168, 178, 1);         // set gateway to match your network


// ========================================================================================
// ws2812b
int mCountLights = 16;                                       // Menge der verwendeten RGB LEDs
int mPinRgbStripe = 4;                                       // Verwenderter Pin für die RGB LED Streifen

// Instanz initialisieren
// Parameter Angaben:
// 1. Anzahl für verwendeter RGB LEDs
// 2. Auf welchen Pin soll verwendet werden.
// 3. Auswahl zwischen RGB, GRB oder BRG und verwendeter Takt 400KHz oder 800KHz
Adafruit_NeoPixel mPixels = Adafruit_NeoPixel(mCountLights, mPinRgbStripe, NEO_RGB + NEO_KHZ800);

  HTTPClient http;
  
void setup() {
  
  Serial.begin(115200);
  
  Serial.print("WiFi connecting to ");
  Serial.println(mSsid);

  Serial.print(F("Setting static ip to : "));
  Serial.println(mIp);

  IPAddress subnet(255, 255, 255, 0);                   // set subnet mask to match your network
  WiFi.config(mIp, mGateway,subnet); 
  WiFi.begin(mSsid, mPassword);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("connect to wifi...");
    delay(500);
  }

  mPixels.begin();
}

void loop() {

  Serial.println("----------------");
  Serial.println(" MESSAGES");
  Serial.println("try http client...");
  
  http.begin("http://192.168.178.65:5000/deviceGet?id=" + DEVICE_NUMBER);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(payload);
  http.end();

  const size_t capacity = JSON_ARRAY_SIZE(1) * JSON_OBJECT_SIZE(5);           
  StaticJsonDocument<capacity> doc;
  deserializeJson(doc, payload);

  Serial.print("doc length: ");
  Serial.println(doc.size(), DEC);

  if(doc.size() == 0) {
    for(int i = 0; i < 10; i++) {
      mPixels.setPixelColor(0, mPixels.Color(255, 0, 0));    
      mPixels.show();
      delay(100);
      mPixels.setPixelColor(0, mPixels.Color(0, 0, 0));    
      mPixels.show();
      delay(100);
    }

    //ESP.reset();
    for(int i = 0; i < 10; i++) {
Serial.print("Try in "); Serial.print(10 - i , DEC); Serial.println(" s");
      delay(1000);
    }
    return;
  }

  bool deviceItemSuccess = doc["Success"];
  Serial.print("Success: "); Serial.println(deviceItemSuccess, BIN);

String deviceItemIdStr = doc["ID"];
  Serial.print("ID: "); Serial.println(deviceItemIdStr);
  
  String deviceItemContent = doc["Content"]; 
  Serial.print("Content: "); Serial.println(deviceItemContent);
  String deviceItemValueStr = doc["Value"];
  char deviceItemValue[deviceItemValueStr.length()];
  strcpy(deviceItemValue, deviceItemValueStr.c_str());
  long receivedValue = atol(deviceItemValue); 
  Serial.print("Value: "); Serial.println(receivedValue, DEC);

  String deviceItemText = doc["Text"];
  Serial.print("Text: "); Serial.println(deviceItemText);

  byte valueUnused = (int)((receivedValue >> 24) & 0xFF);
  byte valueRed = (int)((receivedValue >> 16) & 0xFF);
  byte valueGreen = (int)((receivedValue >> 0) & 0xFF);
  byte valueBlue = (int)(receivedValue & 0xFF);

  mPixels.setPixelColor(0, mPixels.Color(valueRed, valueGreen, valueBlue));    
  
  if(valueUnused == 1){
    for(byte fadeIn = 0; fadeIn < 255; fadeIn++) {
      mPixels.setPixelColor(0, mPixels.Color(fadeIn, fadeIn, fadeIn));  
      mPixels.show();
      delay(2);
    } 
    delay(300);
    for(byte fadeOut = 254; fadeOut > 0; fadeOut--) {
      mPixels.setPixelColor(0, mPixels.Color(fadeOut, fadeOut, fadeOut));  
      mPixels.show();
      delay(2);
    }  
  }

  if(valueUnused == 2) {
    for(int index = 0; index < mCountLights; index++) {
      mPixels.setPixelColor(index, mPixels.Color(150, 0, 0));  
      mPixels.show();
      delay(50);
      mPixels.setPixelColor(index, mPixels.Color(0, 0, 0));  
      mPixels.show();
      delay(30);
    }
  }

  mPixels.show();

  delay(5000);
}
