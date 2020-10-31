#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial blue(11, 12); //bluetooth

String ssid = "-";
String PASSWORD = "-";
String host = "-"; // 서버

char PW[7];
int index = 0;
int count = 0;
int tru = 0;
int pos = 0;
int error = 0;
int Buzzer = 48; 
int mg = 51;
int button = 2;
String state;

int red = 23;
int green = 25;
int bluee = 27;

const byte ROWS = 4; // 행
const byte COLS = 3; // 열

char keys[ROWS][COLS]={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'},
};

byte rowPins[ROWS]={7,6,5,4};
byte colPins[COLS]={10,9,8};
//키패드에 연결된 핀번호 설정(데이터시트 참고)

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

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
  delay(5000);

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

void httpclient(String char_input)
{
  delay(100);
  Serial.println("Connect TCP...");
  Serial3.println("AT+CIPSTART=\"TCP\",\""+host+"\",8283");
  delay(500);

  if(Serial.find("ERROR")) return;

  Serial.println("Send Data");
  
  String url= char_input;
  String cmd= "GET /capd/joljac.php?state="+url+" HTTP/1.0\r\n\r\n";

  Serial3.print("AT+CIPSEND=");
  Serial3.println(cmd.length());
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());

  if(Serial3.find(">"))
  {
    Serial.print(">");
  }
  else
  {
    Serial3.println("AT+CIPCLOSE");
    Serial.println("Connect Timeout");
    delay(500);
    return;
  }
  delay(500);

  Serial3.println(cmd);
  Serial.println(cmd);
  delay(500);

  if(Serial.find("ERROR")) return;

}

void setup(void) 
{  
  Serial.begin(9600);
  blue.begin(9600);
  Serial3.begin(9600);
  
  myservo.attach(13);
  myservo.write(pos);

  lcd.init();
  lcd.noBacklight();

  pinMode(mg, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(button,INPUT);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(bluee, OUTPUT);

  connectWifi();
  delay(500);
  
}
 
void loop(void) 
{
  int a = digitalRead(mg); // 마그네틱 도어센서 읽어서 저장
  if(a == HIGH) // 마그네틱 도어센서가 서로 가까우면
  {
    if(pos == 90) // 문이 열려있는 상태이면
    {
      for (pos = 90; pos > 0; pos -= 1)
     {
       myservo.write(pos);
       delay(15);
      }

    digitalWrite(green, HIGH);
    delay(500);
    
    lcd.backlight();      
    
    tone(Buzzer, 659); 
    delay(100);
    tone(Buzzer, 587); 
    delay(100);
    tone(Buzzer, 523); 
    delay(100);
    noTone(Buzzer);
    delay(500);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Door CLOSE ");
    
    digitalWrite(green, LOW);
    
    blue.write("close");
    state = "close";
    String str_output = String(state);
    delay(500);
    httpclient(str_output);
    delay(500);

    lcd.clear();
    lcd.noBacklight();
    
    }    
  }
  
  while(blue.available()) 
  {
    digitalWrite(bluee, HIGH);
    delay(500);
    
    lcd.backlight();
    
    char receivechar = (char)blue.read();
    PW[index] = receivechar;
    index++;

    lcd.setCursor(0,0);
    lcd.print(" Enter PW ");
    delay(50); 

    digitalWrite(bluee, LOW);
  }

  if (PW[index] == '\0')
  {
    if (index == 6)
    {
      Serial.println(index);
      Serial.println(PW);
    }   
  }
  index= 0;
  delay(50);

  char key = keypad.getKey();
  
  if(key) // key 값이 들어오면
  {
    lcd.setCursor(0,1);
    lcd.print("PW : ");
    delay(50);
    
    Serial.print(key);
    
    if(key==PW[count])
    {
      if(count>0)
      {
        lcd.setCursor(4+count,1);
        lcd.print('*');
      }
      lcd.setCursor(5+count,1);
      lcd.print(key);
      
      count++;
      tru++; 
    }
    else if(key!=PW[count])
    {
      if(count>0)
      {
        lcd.setCursor(4+count,1);
        lcd.print('*');
      }
      lcd.setCursor(5+count,1);
      lcd.print(key);
      
      count++;
    }
    
    if(key=='#')
    {
      re();
    }
       
    if(count==6)
    {
      if(tru==6)
      {
        Su();
        
        lcd.clear();
        lcd.noBacklight();
      }
      else
      {
        Fa();
      }
    } 
  }


  int b = digitalRead(button);// 버튼 읽어서 저장
  if(b==HIGH)//버튼을 누르면
  {
    lcd.clear();
    lcd.backlight();
    
    Su();
    delay(500);

    lcd.clear();
    lcd.noBacklight();  
  }
   
}

void Su()
{
  digitalWrite(green, HIGH);
  delay(500);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Door OPEN ");

  tone(Buzzer, 523); 
  delay(100);
  tone(Buzzer, 587); 
  delay(100);
  tone(Buzzer, 659); 
  delay(100);
  noTone(Buzzer);
  delay(500);  
  
  digitalWrite(green, LOW);

  for (pos = 0; pos < 90; pos += 1)
  {
    myservo.write(pos);
    delay(15);
  }
  
  Serial.println(" 열림");
  
  blue.write("open");
  state = "open";
  String str_output = String(state);
  delay(500);
  httpclient(str_output);
  delay(500);

  error=0; 
  for(int i=0; i<7; i++)
  {
    PW[i]=0;
  } // 열리면 번호 버리기  
  
  tru=0;
  count=0;
  index= 0;  
}

void Fa()
{  
  Serial.println(" 번호 오류");

  digitalWrite(red, HIGH);
  delay(500);
  digitalWrite(red, LOW);
  
  lcd.setCursor(0,0);
  lcd.print(" Re Enter PW ");
  
  error++;
  
  lcd.setCursor(0,1);
  lcd.print(" Error Num: ");
  lcd.setCursor(12,1);
  lcd.print(error);
  delay(5000);
  
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("PW : ");
  delay(50);
  
  if(error==3)
  {
    lcd.clear();
    blue.write("error");
    state = "error";
    String str_output = String(state);
    delay(500);
    httpclient(str_output);
    delay(500);
    
    Serial.println(" 번호를 다시 받으세요.");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" New PW ");
  
    for(int i=0; i<7; i++)
    {
    PW[i]=0;
    } // 번호 버리기
    error=0; 
  }
  
  tru=0;
  count=0;
}

void re()
{
  tru=0;
  count=0;
  Serial.println("번호 초기화");

  digitalWrite(red, HIGH);
  delay(500);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Re Enter PW ");

  digitalWrite(red, LOW);
}
