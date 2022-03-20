//Настройки (активируются когда не смог соединиться с роутером) поднимается точка доступа, можно зайти на 192.168.4.1 и ввести название сети и пароль, после чего перезагрузить и соединиться с роутером.
void wifi_point()//При не удачном запуске запускаем точку доступа
{
   //WiFi.disconnect();
    WiFi.mode(WIFI_STA);//Режим точки доступа
    WiFi.softAP(ssid, password); //Установка названия точки и пароля
      Serial.print("AP IP address: ");
      IPAddress myIP = WiFi.softAPIP();//Получаем айпи своей есп
      Serial.println(myIP);
      server.on("/", handleRoot);//Обратная функция (страница при открытии) 

      /*server.onNotFound([](){                                                 // Описываем действия при событии "Не найдено"
      if(!handleFileRead(server.uri()))                                       // Если функция handleFileRead (описана ниже) возвращает значение false в ответ на поиск файла в файловой системе
          server.send(404, "text/plain", "Not Found");                        // возвращаем на запрос текстовое сообщение "File isn't found" с кодом 404 (не найдено)
      });*/
    
    server.on("/save", save);
    server.begin(); //Запускаем наш HTTP 
    Serial.println("HTTP server started");
}


//------------------------Чтение из EEPROM-------------------------
String read_word(int addr)
{
  String word1;
  char readChar;
  int i = addr;
  
  while (readChar != 123)
  {
    readChar = char(EEPROM.read(i));
    delay(10);
    i++;
     
    if (readChar != 123)
    {
      word1 += readChar;
     
    }
     if((i-addr) > 40) break;
  }
  return word1;
}
//---------------------Запись в EEPROM------------------------------
void write_word(int addr, String word)
{
  delay(10);
   for (int i = 0; i < word.length(); ++i) {
      Serial.print(word[i]);
      EEPROM.write(addr+i, word[i]);
    }
  Serial.println();
  EEPROM.write(addr+word.length(), 123);
  EEPROM.commit();
  Serial.println("saved");
}

//--------------------после нажатия на кнопку save--(страницы)---------
void save()
{
   //    server.send(200, "text/html", "<head><meta http-equiv='refresh' content='1;URL=http://192.168.4.1'/> </head>"); Перенаправить обратно
  if ((server.arg("pas") != "") and (server.arg("name_wifi")!="")){
             write_word(1,server.arg("name_wifi"));
             write_word(100,server.arg("pas"));
      }else  server.send(200, "text/html", "<head><meta http-equiv='refresh' content='1;URL=http://192.168.4.1/?save=no'> </head>");// Перенаправить обратно если одно из полей пустое
      //server.send(200, "text/html", "<h1>Saved! reboot device</h1>"+server.arg("wifi")+"<hr>"+server.arg("pas"));
      server.send(200, "text/html", "<head><meta http-equiv='refresh' content='1;URL=http://192.168.4.1/?save=yes'> </head>");
}

//--------------Главная страница--ввод названия и пароля сети-------------------------------------
void handleRoot() {

int n = WiFi.scanNetworks(); //Количество найденых точек доступа

  String message;
    if (n == 0) {
       message +=("no networks found");
    } else {
       
        
        //html Страничка при первом включении для поиска к какой точке доступа подключаться (сканируем точки вокруг, выбираем нужную вводим пароль и сохраняем) 
          message +=("<html lang='ru'>");
          message +=(" <head>");
              message +=("<meta charset='utf-8'/>");
              message +=("<meta name='viewport' content='width=device-width, initial-scale=1.0'/>"); 
            
              message +=("<style type='text/css'>");
              message +=("       body{ background:black;  color:white;  text-align:center;} ");
              message +=("</style>");  //<meta name="viewport" content="width=device-width, initial-scale=1.0"> <!--Строчка, которая делает магию-->
            // message +=("<meta name='viewport' content='target-densitydpi=device-dpi' />"); 
             // message +=("<meta name='HandheldFriendly' content='true'/>");
              message +=("<title>Setup</title>");
          message +=("</head>");
       
          message +=("<body>");
              message +=("<br><br><br><br><br><br><br><br><br><br><br><br><h1>Found WiFi networks:"+String(n)+"</h1>");
              message +="<form action='/save'>";
              message += "<label for='wifin'>wi-fi:</label>";
              message += "<input type='text' name='name_wifi' list='wifilist' required /> ";
              message += "<datalist id='wifilist'>";
                            for (int i = 0; i < n; ++i) {
                                                            message += "<option value='"+WiFi.SSID(i)+"'>"+WiFi.SSID(i)+"["+WiFi.RSSI(i)+"] pass("+((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*")+")</option>";
                                                            delay(10);
                                                        }
          }
             
             message += "</datalist>";

          

            
             message += "password: <input type='password' name='pas' size='15' required><br><br><br>";
             message +="<input type='submit' value='Save'>";
             message +="</form>"; 

              if(server.arg("save") == "yes") {
                  message +="saved! Please restart devace!";  
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
