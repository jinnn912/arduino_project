#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define _SS_MAX_RX_BUFF 512
#define serviceKey String("-") // serviceKey
#define weatherReqInterval 3600000 // 60초 * 60분 * 1000(밀리세컨드)

String ssid = "-";
String PASSWORD = "-";

int cmdSize = 0;
String rcvbuf;
String weatherStr;
String tempStr;
float tempFloat;
boolean weatherFlag = false;
boolean tempFlag = false;
boolean weatherDisp = false;
boolean tempDisp = false;
unsigned long prevConMillis = 0;

void setup(void)
{
  Serial.begin(9600);
  Serial3.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connecting");

  connectWifi();

  /*

  Serial.println("Getting weather");
  weatherReq();
  delay(1000);
  
  while(Serial3.available()){
    if(rcvbuf.endsWith("\"description\":\"")){
      weatherFlag = true;
      weatherStr = "";
    }
    
    if(rcvbuf.endsWith("\"temp\":")){
      tempFlag = true;
      tempStr = "";
    }

    char c = Serial3.read();
    //Serial.write(c);
    if(c != NULL){
      rcvbuf += c;
    }
    Serial.print(c);

    if(weatherFlag){
      if(c != '\"'){
        weatherStr += c;
      }else{
        weatherFlag = false;
        weatherDisp = true;
      }
    }
    
    if(tempFlag){
      if(c != ','){
        tempStr += c;
      }else{
        tempFloat = tempStr.toFloat() - 273.0;
        tempFlag = false;
        tempDisp = true;
      }
    }
  }

  if(tempDisp && weatherDisp){
    Serial.println(weatherStr);
    Serial.println(tempFloat);

    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print(weatherStr);

    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print(tempFloat);

    weatherDisp = false;
    tempDisp = false;
  } else {
    Serial.println("failed");
  }
  */
}

void loop(void){
  Serial.println("[INFO] Getting weather...");
  weatherReq();
  delay(1000);
  
  while(Serial3.available()){
    if(rcvbuf.endsWith("\"description\":\"")){
      weatherFlag = true;
      weatherStr = "";
    }
    
    if(rcvbuf.endsWith("\"temp\":")){
      tempFlag = true;
      tempStr = "";
    }

    char c = Serial3.read();
    Serial.write(c);
    if(c != NULL){
      rcvbuf += c;
    }

    if(weatherFlag){
      if(c != '\"'){
        weatherStr += c;
      }else{
        weatherFlag = false;
        weatherDisp = true;
      }
    }
    
    if(tempFlag){
      if(c != ','){
        tempStr += c;
      }else{
        tempFloat = tempStr.toFloat() - 273.0;
        tempFlag = false;
        tempDisp = true;
      }
    }
  }

  if(tempDisp && weatherDisp){
    Serial.println(weatherStr);
    Serial.println(tempFloat);

    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print(weatherStr);

    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print(tempFloat);

    weatherDisp = false;
    tempDisp = false;
  } else {
    Serial.println("[ERROR] Failed");
  }
}

void weatherReq(){
  String str = "GET /data/2.5/weather?lat=37.56826&lon=126.977829&APPID=" + serviceKey;
  //String str = "GET /data/2.5/weather?q= Incheon,kr&&APPID=" + serviceKey;
  str.concat(" HTTP/1.0\r\nHost:api.openweathermap.org\r\nConnection: close\r\n\r\n");
  cmdSize = str.length();
  Serial3.println("AT+CIPSTART=\"TCP\",\"api.openweathermap.org\",80");
  delay(500);
  Serial3.print("AT+CIPSEND=");
  delay(500);
  Serial3.println(cmdSize);
  delay(500);
  Serial3.println(str);
  prevConMillis = millis();
}

void connectWifi()
{
  String join ="AT+CWJAP=\""+ssid+"\",\""+PASSWORD+"\"";

  Serial3.println("AT+CWMODE=1");
  delay(500);
  while(Serial3.available()){
    Serial.write(Serial3.read());
  }

  Serial3.println("AT+CIPMUX=0");
  delay(500);
  while(Serial3.available()){
    Serial.write(Serial3.read());
  }
  
  Serial.println("Connect WiFi");
  Serial3.println(join);
  delay(10000);

  if(Serial3.find("OK"))
  {
    Serial.print("WiFi Connect\n");
  }
  else
  {
    Serial.println("Connect Timeout\n");
  }
  delay(5000);

  while(Serial3.available()){
    Serial.write(Serial3.read());
  }
}
