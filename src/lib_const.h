//-------Для поднятия точки доступа--------------



#define period_time 40*1000           //Таймер
#define interruptPin 5             //Пин прерывания


#define OTAUSER         "admin"    // Логин для входа в OTA
#define OTAPASSWORD     "admin"    // Пароль для входа в ОТА
#define OTAPATH         "/firmware"// Путь, который будем дописывать после ip адреса в браузере.
#define SERVERPORT      80         // Порт webserverа 
#define TIME_CONNECT    40         //по пол секунды на попытку (на странице conect_wifi)
