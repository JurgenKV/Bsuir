#include <Keypad.h>


#include <SoftwareSerial.h>

#define  blueth_RX 0
#define  blueth_TX 1
const int pinButtonLed = 2;
const int pinButtonReloadBlth = 3;
#define PIN_POT A4
bool boxLed = false;
int rotate;

SoftwareSerial mySerial (blueth_TX, blueth_RX);
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'A', 'B', 'C', 'D'},
  {'E', 'F', 'G', 'H'},
  {'I', 'J', 'K', 'L'},
  {'M', 'N', 'O', 'P'}
};

byte colPins[COLS] = {9, 10, 11, 12}; //к каким выводам подключаем управление строками
byte  rowPins[ROWS] = {5, 6, 7, 8}; //к каким выводам подключаем управление столбцами

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  mySerial.begin(9600);
  pinMode(4, OUTPUT);
  rotate = analogRead(PIN_POT);
}

void loop() {

  char customKey = customKeypad.getKey();//записывем нажатый символ
  int buttonStateLed = digitalRead(pinButtonLed);
  int buttonStateReload = digitalRead(pinButtonReloadBlth);

  //mtx
  if (customKey) { //если что-то нажато
    mySerial.println(customKey);
  }
  ///

  /////////////////////// LED
  if (buttonStateLed) {
    boxLed = boxButtonSettingsLed(boxLed);
    delay(1000);
  }
  /////////////////////////blth reload
  if (buttonStateReload) {
    bluetoothReload();
  }
  ///

  rotate = sendRotate(analogRead(PIN_POT));
}

bool boxButtonSettingsLed(bool flag ) {
  if (!flag) {
    digitalWrite(4, HIGH);
    flag = true;
  } else {
    digitalWrite(4, LOW);
    flag = false;
  }
  delay(10);
  return flag;
}

void bluetoothReload() {
  mySerial.end();
  delay(1000);
  mySerial.begin(9600);
  delay(1000);
}

int sendRotate(int rt) {

  if (rt > (rotate + 50) ) {
    mySerial.println('Q');
    delay(300);
    return rt;
  }
  if (rt < (rotate - 50)) {
    mySerial.println('R');
    delay(300);
    return rt;
  }
  return rotate;
}
