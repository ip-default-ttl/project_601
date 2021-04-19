#include <DHT.h>
#define PIR 13
#define LED 6
int ledPin = 13;
byte incomingByte;
bool b=true;
#include <Servo.h>
Servo eservo;
#define PIN_TRIG 12
#define PIN_ECHO 11
long duration, cm;
int cm1 = 0;
int cm2 = 0;
#define smokePin A0    // определяем аналоговый выход к которому подключен датчик
int sensorThres = 40; // пороговое значение АЦП, при котором считаем что газ есть
const int LED_RED = 10;              // Порт 10, красный светодиод
const int LED_YELLOW = 9;           // Порт 9, желтый светодиод
const int LED_GREEN = 8;             // Порт 8, зеленый светодиод
int p = 6;
const int TIMEOUT_GREEN = 3000;
DHT dht(2, DHT11);  // сообщаем на каком порту будет датчик
bool ON1; //переменная сохраняет состояние светодиода
bool ON2; // для каждого светодиода своя переменная
bool ON3;
int f1();
int n=0;
void keydelay(unsigned long a)
{
  unsigned long long int time = millis();
  while (millis()<time+a)
  {
    String a="";
    a=Serial.readString();

    if(a == "tem"){
      digitalWrite(ledPin, HIGH);
      float t = dht.readTemperature();
      Serial.print("Температура: ");
      Serial.print(t);
    }
    if (a == "hum"){
      digitalWrite(ledPin, HIGH);
      float h = dht.readHumidity();
      Serial.print("Влажность: ");
      Serial.print(h);

    }
    if ( a == "smoke") {
      digitalWrite(ledPin, HIGH);
      Serial.print("Газ: ");
      int analogSensor = analogRead(smokePin);
      Serial.print(analogSensor);
    }

    if ( a == "echo") {
      f1();
      digitalWrite(ledPin, HIGH);
      Serial.print("Расстояние: ");
      Serial.print(cm);
    }
    if (a == "ser1"){
      {
        digitalWrite(LED_RED, LOW);
        keydelay(1000);
        digitalWrite(LED_YELLOW, HIGH);
        keydelay(1000);
        digitalWrite(LED_YELLOW, LOW);
        eservo.write(90);
        digitalWrite(LED_GREEN, HIGH);
        keydelay(1000);

        f1();
        while (cm < 10)
        {
          tone (p, 500, 500);
          f1();


        }
      }
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      keydelay(1000);
      digitalWrite(LED_YELLOW, LOW);
      keydelay(1000);
      digitalWrite(LED_RED, HIGH);
      eservo.write(10);
    }

    if (a == "ser2") {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);
      eservo.write(0);
    }
    if (a ==  "ser3") {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_GREEN, LOW);
      eservo.write(100);
    }
    if ( a == "1") {
      digitalWrite(ledPin, HIGH);
    }
    if ( a ==   "move1") {
      int pirVal = digitalRead(PIR);//  переменная для датчика движения
      if (pirVal == HIGH) {
        Serial.print("1");
        digitalWrite(LED, HIGH);
        //delay(100);
      }
      else {
        Serial.print("0");
        digitalWrite(LED,LOW);

        //delay(100);
      }
    }
    else if (a == "0"){
      digitalWrite(ledPin, LOW);
    }
  }
}



unsigned long VREMYA1 = millis(),VREMYA2,VREMYA3; //считаем время с начала работы контроллера

void setup() {
  eservo.write(10);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(ledPin, OUTPUT);
  dht.begin();                // запускаем датчик DHT11
  Serial.begin(9600);
  eservo.attach(7);

  // Все порты светодиодов будут у нас установлены в режим "внешняя нагрузка", OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  // Устанавливаем начальное значение светодиодов
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  pinMode(PIR, INPUT);
  pinMode(LED,OUTPUT);
  keydelay(1000000);
}
void loop() {
  f1();
  int pirVal = digitalRead(PIR);//  переменная для датчика движения

  int analogSensor = analogRead(smokePin);
  //Serial.print (analogSensor);
  float h = rand();//dht.readHumidity();
  float t = rand();//dht.readTemperature();
}

int f1() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);
  // Выставив высокий уровень сигнала, ждем около 10 микросекунд. В этот момент датчик будет посылать сигналы с частотой 40 КГц.
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  //  Время задержки акустического сигнала на эхолокаторе.
  duration = pulseIn(PIN_ECHO, HIGH);

  // Теперь осталось преобразовать время в расстояние
  cm = (duration / 2) / 29.1;
  return cm;
}
