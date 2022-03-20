void Reciev() 
 {
  String st;
 while (socket_server.available()) {
      char ch = static_cast<char>(socket_server.read());
      if (ch != '\r') 
       {
         st = st+ch;
          
       } else 
         {
           if (st == "who") 
            {
               Serial.println("I am Arduino");
               socket_server.println("Arduino2");
            }
           Serial.print("Server:");
           Serial.println(st);
         }
         
      if (ch == 'y')
        {
          socket_server.println("/arduino");
         // Serial.println("yes otv"); 
        }

        if (ch == '1')
        {
          socket_server.println("vkl");
         // Serial.println("vkl");
          digitalWrite(2,1); 
        }

            if (ch == '0')
        {
          socket_server.println("vikl");
         // Serial.println("vikl");
          digitalWrite(2,0); 
        }
        
      //client.stop();
      //Serial.println(ch);
    }

 }
