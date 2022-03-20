#include "ESP8266WiFi.h"                
#include <WiFiClient.h>                 // Клиент вай-фай (подключение к сети)
#include <ESP8266WebServer.h>           // Веб сервер(отображение html страниц)
#include <ESP8266HTTPUpdateServer.h>    // Обновление прошивки по воздуху
#include <EEPROM.h>                     // Постоянная память (там хранится логин и пароль сети)
#include "lib_const.h"                  // все костанты 
#include <FS.h>                         // Файловая система (при необходимоти)
#include <PubSubClient.h>               // Mqtt 

volatile uint8_t count=0;
volatile unsigned long timePrew=0;
volatile bool flag=0;
bool conn;
String MQTT_IP;

//-------------config lan----------------
String hostIPstr = "192.168.10.115"; //Указываем для веб (html)
IPAddress hostip(192, 168, 10, 115); //IP Адрес есп
IPAddress mask(255, 255, 255, 0);    //Подсеть
IPAddress gateway(192, 168, 10, 1);  // Шлюз 
IPAddress dns1(8, 8, 8, 8);          //Днс1
IPAddress dns2(192, 168, 10, 1);     //Днс2
//-----------------config lan--------------

//----------mqtt----------
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
//----------mqtt----------

//-------Для поднятия точки доступа--------------
#ifndef APSSID
#define APSSID "Esp_8266"
#define APPSK  "12345678"
#endif


/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
//-------Для поднятия точки доступа--------------

//Mqtt---------------------------------
//IPAddress IPMqttServer //(192, 168, 10, 118);
uint16_t Mqtt_port; // = 1883;
//-------Setting-socket-server----
String IPSocketServer;// = "192.168.10.118";
uint16_t Socket_port;// = 6000;
//-------Setting-socket-server----



String namenetwork=""; //Имя и пароль берем из EEPROM 
String pass = "";
//unsigned long period_time = (long)2000;
unsigned long my_timer;// переменная таймера, максимально большой целочисленный тип (он же uint32_t)


//Прерывания
 void ICACHE_RAM_ATTR test()
 {
   if ((millis() - timePrew ) >= 400)
    {
      flag=1; 
      timePrew= millis();  
    }

   if(flag == 1) 
    {
       count++;
       flag=0;
       digitalWrite(2,!digitalRead(2));
    }   
 }


WiFiClient client;//Для Mqtt
WiFiClient socket_server; //для сокет сервера
PubSubClient Mqtt_client(client);

ESP8266WebServer server(SERVERPORT);//Веб интерфейс на 80 порту 
ESP8266HTTPUpdateServer httpUpdater;//экземпляр класса обновление системы по воздуху


void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  //SPIFFS.begin();  // Инициализируем работу с файловой системой  
   
  pinMode(interruptPin, INPUT_PULLUP); 
  pinMode(2, OUTPUT);  //Светодиод 
 
  attachInterrupt(digitalPinToInterrupt(interruptPin),test, RISING); //FALLING RISING Прерывания

  
//--------------wifi------------------------------------------------   
  namenetwork = read_word(1);//Читаем из ЕЕПРОМ имя сети точки доступа
  pass = read_word(100);//Читаем из ЕЕПРОМ пароль точки доступа
  conn =  connect_wifi(namenetwork,pass,1); //подключение к вай-фай
//--------------wifi------------------------------------------------


//-------------Mqtt-------------------
  MQTT_IP =  read_word(150); //Читаем из ЕЕПРОМ адрес 
  unsigned char* buf = new unsigned char[50]; //Буфер для адреса
  MQTT_IP.trim(); //Обрезаем лишнии пробелы (они там есть))
  MQTT_IP.getBytes(buf, 50, 0);// Перебрасываем адрес
  const char *mqtt_ip = (const char*)buf;

  EEPROM.get(200, Mqtt_port); //Читаем порт

  Mqtt_client.setServer(mqtt_ip, Mqtt_port); //Указываем сервер MQTT
  Mqtt_client.setCallback(callback);
//
//    Serial.print("Mqtt-server:");
//  Serial.println(mqtt_ip);
//  Serial.print("Mqtt-port:");
//  Serial.println(Mqtt_port);

//Mqtt_client.connect("156387","te","123");
Mqtt_client.connect("156387","te","1234");
//------Mqtt--------------------------------------------------------------


//-------------Socket-------------------
  IPSocketServer = read_word(250); //IP адррес сокета читаем как строку 
  EEPROM.get(300, Socket_port);     //Port сокета читаем как число

//  Serial.print("socket host:");    
//  Serial.println(IPSocketServer);
  Serial.print("socket-port:");
  Serial.println(Socket_port);
//-------------Socket-------------------


  Serial.println("start");
  Serial.println(conn); 
  my_timer = millis();   // "сбросить" таймер
}


void loop() {
 server.handleClient();//Веб интерфейс

  //------------------------------------------MQTT-----------------------------------------------------
   if (Mqtt_client.connected()) {
    //reconnect(); //Переподключение MQTT
   // Mqtt_client.publish("outTopic", "hello world");
  }

  Mqtt_client.loop();
  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    Mqtt_client.publish("outTopic", msg);
  }
//------------------------------------------MQTT-----------------------------------------------------






//------------------------------------------WI-FI-----------------------------------------------------
  //Если нету подключения к wi-fi то пытаемся подключиться (больше пока не будет дальше код не идет)
  if ((WiFi.status() != WL_CONNECTED) /*and (conn==1)*/) 
    {
        WiFi.reconnect();//После долгих мучений и попыток костылей, нашел в библиотеке метод на случай, по этому просто используем его
        Serial.print("disconnect _ reconnect");//Работает авто подключение, но на всякий случай код оставил, на моем роутере не всегда срабатывает)
      //connect_wifi(namenetwork,pass,0);//Пытаемся подключиться к вай фай но без поднятия точки доступа, просто постоянно пытаемся подключиться в случае обрыва
       return;//нету смысла в продолжении без wifi (выше нужно организовать проверку кнопок что бы работало без сети)
    }
//------------------------------------------WI-FI-----------------------------------------------------
    






 
//------------------------------------------Socket-----------------------------------------------------
    //Если нету подключение к сокет серверу то пишем об этом и не продолжаем  
    if (!socket_server.connected())
    {
      // Serial.println("no connect");
       socket_server.connect(IPSocketServer,Socket_port);
       //delay(1000);
       return;
    }

    Reciev();//Прием сообщений 

  
   //Каждые N секунд отправка сообщения по сокету!
   if (millis() - my_timer >= period_time) {
     my_timer = millis();   // "сбросить" таймер
       
        //Serial.print("count:");//Счетчик для прерывания (по пину 5)
       // Serial.println(count);
      //  count++;
        if (count % 4 == 0 ) {
         
            socket_server.println("Ping");
        }
     }
//------------------------------------------Socket-----------------------------------------------------

    








   

}
