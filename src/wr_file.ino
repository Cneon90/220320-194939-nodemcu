 // always use this to "mount" the filesystem
 
String wr_read(String fileName) 
{   
  bool result = SPIFFS.begin();
  String st ;  
 // Serial.println("SPIFFS opened: " + result);

  // this opens the file "f.txt" in read-mode
  File f = SPIFFS.open(fileName, "r");

  if (!f) {
    Serial.println("File doesn't exist yet. Creating it");
    exit;
  }

  
    // now write two lines in key/value style with  end-of-line characters
    f.println("ssid=abc");
    f.println("password=123455secret");
  
    
    // we could open the file
    while(f.available()) {
      //Lets read line by line from the file
      String line = f.readStringUntil('\n');
      st = st+line;
      
    }

  
  f.close();
  
  return st;
}


bool wr_write(String fileName)
{

   bool result = SPIFFS.begin();
   File f = SPIFFS.open("/f.txt", "w");
    if (!f) {
      Serial.println("file creation failed");
     return 0;  
    }
    // now write two lines in key/value style with  end-of-line characters
    f.println("ssid=Cneon90");
    f.println("password=9bbed9c328");

  return 1;  
}
