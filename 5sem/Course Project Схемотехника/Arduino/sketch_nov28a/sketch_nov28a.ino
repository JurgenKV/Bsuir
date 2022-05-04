#define  ledPinWork 2
#define  ledPinGercon3 3
#define  ledPinPhoto4 4
#define  ledPinZymer5 5
#define  ledPinLaser6 6
#define  ledPinAll7 7
#define  ledPinAll8 8
#define  ledPinAll9 9
#define  gerconPin 10
#define  laserPin 11
#define  zymerPin 12
#define  PIN_PHOTO_SENSOR A1

byte gercon; // информация о герконе
byte laser; // информация о лазере
bool zym; // состоянние зумера
bool gerconState; // состояние геркона
bool photoState; // состояние фоторезистора

void setup() {
  pinMode(ledPinWork,OUTPUT);   //подключение всех используемых пинов
  pinMode(ledPinGercon3,OUTPUT);
  pinMode(ledPinPhoto4,OUTPUT);
  pinMode(ledPinZymer5,OUTPUT);
  pinMode(ledPinLaser6,OUTPUT);
  pinMode(ledPinAll7,OUTPUT);
  pinMode(ledPinAll8,OUTPUT);
  pinMode(ledPinAll9,OUTPUT);
  pinMode(gerconPin,INPUT);
  pinMode(laserPin,OUTPUT);
  pinMode(zymerPin,OUTPUT);
}

void loop() {
   digitalWrite(ledPinWork, HIGH); // состояние исправности схемы
   gerconInit(); // функция работы с герконом 
   photoInit(); // функция работы с фоторезистором
   zymerInit(); // функция работы с зумером
   laserState(); // функция просмотра состояния лазера
   isAllWork(); // функция оповещения о срабатывании всех защитных модулей
}

void gerconInit(){
 gercon = digitalRead(gerconPin);        // считываем данные с датчика
   if (gercon == HIGH){ // вклююченние зумера и индексации, при HIGH
      digitalWrite(zymerPin, HIGH); // включение зумера 
      zym = true;
      gerconState = true;
      digitalWrite(ledPinGercon3, HIGH);  // переключение режимов светодиода
      delay(500);
      digitalWrite(ledPinGercon3, LOW);
      delay(500);
      }
   else{
      digitalWrite(zymerPin, LOW); // Режим ожидания
      zym = false;
      gerconState = false;
      digitalWrite(ledPinGercon3, HIGH); // постоянное горение светодиода
   }
  }
  
void photoInit(){
   digitalWrite(laserPin, HIGH);
   int val = analogRead(PIN_PHOTO_SENSOR); // считывание информации фоторезистора
  Serial.println(val); // вывод информации в консоль
  if (val > 200) {  // условие срабатывания зумера, при отводе лазера с фоторезистора
    digitalWrite(zymerPin, HIGH);
    zym = true;
    photoState = true;
    digitalWrite(ledPinPhoto4, HIGH);  // переключение режимов светодиода
      digitalWrite(ledPinPhoto4, HIGH);
      delay(500);
      digitalWrite(ledPinPhoto4, LOW);
      delay(500);
  } else {
    digitalWrite(ledPinPhoto4, HIGH); // постоянное горение светодиода 
    digitalWrite(zymerPin, LOW);
    photoState = false;
    zym = false;
  } 
  }

  void zymerInit(){
    if(zym == false) // показ состояния зумера на светодиодах
    digitalWrite(ledPinZymer5,HIGH);
    else{
      digitalWrite(ledPinZymer5, HIGH); // переключение режимов светодиода
      delay(500);
      digitalWrite(ledPinZymer5, LOW);
      delay(500);
      }
    }
 void laserState(){
   laser = digitalRead(laserPin); // чтение данных о питании лазера 
   if (laser == LOW){
      digitalWrite(ledPinLaser6, HIGH); // переключение режимов светодиода
      delay(500);
      digitalWrite(ledPinLaser6, LOW);
      delay(500);
      }
   else{
      digitalWrite(ledPinLaser6, HIGH);
   }
 }

 void isAllWork(){
  if( photoState == true and  gerconState == true){ // рассмотр состяния обоих средств защиты
     digitalWrite(ledPinAll7, HIGH); // // переключение режимов светодиода
     digitalWrite(ledPinAll8, LOW);
     digitalWrite(ledPinAll9, HIGH);
     delay(500);
      digitalWrite(ledPinAll7, LOW);
     digitalWrite(ledPinAll8, HIGH);
     digitalWrite(ledPinAll9, LOW);
     delay(500);
  }
  else{
     digitalWrite(ledPinAll7, LOW); // отключение соответствующих светодиодов
     digitalWrite(ledPinAll8, LOW);
     digitalWrite(ledPinAll9, LOW);
     }
 }
 
 
 
  
