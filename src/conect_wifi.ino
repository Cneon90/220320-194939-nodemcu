//Если удалось подключиться к сети, попадам в этот раздел, тут грузиться страница из файловой системы!!!
bool connect_wifi(String ssid,String password,bool oneRun)//Попытки подключиться к роутеру который в EEPROM устройства
{
  Serial.println("Name_wifi:"+namenetwork); //Читаем из памяти значения 
  Serial.println("Password:"+pass);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);//из ЕЕПРОМа береутся значения и подставляются для того что бы попытаться подключиться
    WiFi.config(hostip,gateway,mask,dns1,dns2);
    //WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);//судя по названию должен переподключиться после потери связи с вай-фай
   //WiFi.begin("123", "12345678");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {//Пытаемся подключиться
    delay(500);
 
    digitalWrite(2,!digitalRead(2));
    Serial.print(".");
    i++;
    if (i > TIME_CONNECT) //Если установленное количество раз не получилось, то поднимаем точку доступа(для изменения названия точки доступа и пароля)
     {
        if (oneRun == 1) 
          {wifi_point(); 
           return 0;//Если при старте не удалось подключиться тогда возвращаем 0, что бы не пробовать больше
           break;
          }
          //Если запущена из сетап, тогда поднять точку доступа, если нет, тогда будет постоянно пытаться подключиться к роутеру 
        Serial.println("");
       break; 
     }
     
   }

   
  if(WiFi.status() == WL_CONNECTED) //Успешное подключение к роутеру
  {
      Serial.println("");
      Serial.println("WiFi connected");
      
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      
      Serial.print("subnet Mask: ");
      Serial.println(WiFi.subnetMask());
    
      Serial.print("gateway: "); 
      Serial.println(WiFi.gatewayIP());
      
      Serial.print("dnsIP: "); 
      Serial.println(WiFi.dnsIP());
     

      Serial.print("mac address:"); 
      Serial.println(WiFi.macAddress());
      
      digitalWrite(2,1);
      //connected_wifi=1;
    
     /* server.onNotFound([](){                                                 // Описываем действия при событии "Не найдено"
      if(!handleFileRead(server.uri()))                                       // Если функция handleFileRead (описана ниже) возвращает значение false в ответ на поиск файла в файловой системе
          server.send(404, "text/plain", "Not Found");                        // возвращаем на запрос текстовое сообщение "File isn't found" с кодом 404 (не найдено)
      });
        // HttpServer.onNotFound(handleNotFound);
     */

      Serial.println("welcome to web");
      server.begin(); //Запускаем наш HTTP (здесь запуск происходит после подключения к роутеру)
      httpUpdater.setup(&server, OTAPATH, OTAUSER, OTAPASSWORD);//Настройки обновление по воздуху
      server.on("/", GetSettingMain);//Страница при подключение откроется после подключения к роутеру
      server.on("/mqtt", mqttSave);//
  }
  return 1;//Если при старте было подключение, будем подключаться в случае отключения
}

/* Файловая система
bool handleFileRead(String path){                                       // Функция работы с файловой системой
  if(path.endsWith("/")) path += "index.html";                          // Если устройство вызывается по корневому адресу, то должен вызываться файл index.html (добавляем его в конец адреса)
  String contentType = getContentType(path);                            // С помощью функции getContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову
  if(SPIFFS.exists(path)){                                              // Если в файловой системе существует файл по адресу обращения
    File file = SPIFFS.open(path, "r");                                 //  Открываем файл для чтения
    size_t sent = server.streamFile(file, contentType);                   //  Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
    file.close();                                                       //  Закрываем файл
    return true;                                                        //  Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
  }
  return false;                                                         // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
}


String getContentType(String filename){                                 // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
  if (filename.endsWith(".html")) return "text/html";                   // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
  else if (filename.endsWith(".css")) return "text/css";                // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
  else if (filename.endsWith(".js")) return "application/javascript";   // Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
  else if (filename.endsWith(".png")) return "image/png";               // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
  else if (filename.endsWith(".jpg")) return "image/jpeg";              // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpg" и завершаем выполнение функции
  else if (filename.endsWith(".gif")) return "image/gif";               // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
  else if (filename.endsWith(".ico")) return "image/x-icon";            // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
  return "text/plain";                                                  // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
}

*/

void mqttSave()
{
  uint16_t port;
  String portStr;
   if ((server.arg("mqttserver") != "") and (server.arg("sokethost")!="")){

             write_word(150,server.arg("mqttserver"));
             portStr = server.arg("mqttport");
             port =portStr.toInt();
             EEPROM.put(200, port);

             
             write_word(250,server.arg("sokethost"));
             portStr = server.arg("soketport");
             port = portStr.toInt();
             Serial.println("SOKET_PORT:");
             Serial.println(port);
             EEPROM.put(300,port);
             
//             delay(100);
//             server.send(200, "text/html", "<head>  <h1 {text-align:center}>Please reset device </h1> </head>");// Перенаправить обратно если одно из полей пустое
             Serial.println("Save-mqtt");
             
            
      }else  server.send(200, "text/html", "<head><meta http-equiv='refresh' content='1;URL=http://"+hostIPstr+"/?save=no'> </head>");// Перенаправить обратно если одно из полей пустое
}


void GetSettingMain()
{
  //--------------Главная страница--после подключения к основной сети-------------------------------------
  String message;
  //html Страничка при первом включении для поиска к какой точке доступа подключаться (сканируем точки вокруг, выбираем нужную вводим пароль и сохраняем) 
  message +=("<html lang='ru'>");
  message +=(" <head>");
  message +=("<meta charset='utf-8'/>");
  message +=("<meta name='viewport' content='width=device-width, initial-scale=1.0'/>"); 
  message +=("<style type='text/css'>");
  message +=("       body{ background:black;  color:white;  text-align:center;} ");
  message +=("</style>");  //<meta name="viewport" content="width=device-width, initial-scale=1.0"> <!--Строчка, которая делает магию-->
  message +=("<title>Setting</title>");
  message +=("</head>");
  message +=("<body>");
  message +=("<h1> SETTING </h1>");

 message +=("<h2> Mqtt </h2>");
  message +="<form action='/mqtt'>";
              message += "<label for='lan'>mqtt host:</label>";
             //--------------mqtt-------------------------------------------------------
              //Host
              message += "<input type='text' name='mqttserver' list='lan' required value='"+MQTT_IP+"'/> ";
              message += "<datalist id='lan'>";
              message += "<option value=192.168.1.1 </option>";
              message += "<option value=192.168.10.1 </option>";
              message += "<option value=192.168.100.1 </option>";
              message += "<option value=192.168.0.1 </option>";
              message += "</datalist>";

             //port
             message += "mqtt-port: <input type='text' name='mqttport' size='4' required value='"+String(Mqtt_port)+"'><br><br><br>";
             //--------------mqtt-------------------------------------------------------

             message +=("<h2> Socket </h2>");
             //-------------socket-------------------------------------------------------
             message += "socket host: <input type='text' name='sokethost' size='16' required value="+IPSocketServer+">";
             message += "socket port: <input type='text' name='soketport' size='4' required value='"+String(Socket_port)+"'><br><br><br>";
 
             
             message +="<input type='submit' value='Save'>";
             message +="</form>";

  
   
              if(server.arg("save") == "yes") {
                  message +="saved! Please restart devace";  
              }

              if(server.arg("save") == "no") {
                  message +="ERROR!!!";  
              }

              
  message +=("</body>");
  message +=("</html>");
   //message += "<a href='/save?name=pas'> connect </a> ";
  server.send(200, "text/html", message);
  Serial.println(message);
}

  
