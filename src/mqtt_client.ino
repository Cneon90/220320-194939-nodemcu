//Обратная функция MQTT (Вызывается при приходе сообщения)

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message_mqtt=""; 
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    message_mqtt = message_mqtt + (char)payload[i];
  }
  Serial.print(message_mqtt);
  Serial.println();
    
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

  //----Пример приема сообщения (не символа)
  if (message_mqtt == "kirill") {
    Serial.println("Yess"); 
   }
}

void reconnect() {
  // Loop until we're reconnected
  while (!Mqtt_client.connected()) {
   // Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (Mqtt_client.connect(clientId.c_str())) {
    //  Serial.println("connected");
      // Once connected, publish an announcement...
      Mqtt_client.publish("outTopic", "hello world");
      // ... and resubscribe
      Mqtt_client.subscribe("inTopic");
    } else {
    //  Serial.print("failed, rc=");
    //  Serial.print(Mqtt_client.state());
     // Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}







 
