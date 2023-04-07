

/* --- www.nyebarilmu.com --- */
//#include <Wire.h>
//#include <LiquidCrystal_PCF8574.h>


#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include "EmonLib.h"

// Set your access point network credentials
const char* ssid = "ESP8266-bacaArus";
const char* password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
float aruss;
float dayaa;


EnergyMonitor emon1;

int tegangan = 220.0;

 
//pin yang digunakan sensor SCT
int pin_sct = A0;
 
//LiquidCrystal_PCF8574 lcd(0x38); 
// dapat dicek menggunakan i2c finder



String getTemperature() {
  float temperature = aruss;///32.8;    //bme.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float temperature = 1.8 * bme.readTemperature() + 32;
  Serial.println(temperature);
  return String(temperature);
}


String getHumidity() {
  float humidity = dayaa;//50.5;  //bme.readHumidity();
  Serial.println(humidity);
  return String(humidity);
}


String processor(const String& var){
  Serial.println(var);
  if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }
        //  else if (var == "PRESSURE"){
        //    return AngkaAcak();   //getPressure();
        //  }  
  return "ada yang error";
}





void initLfs(){
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS"); 
  }
  Serial.println("SPIFSS aktiv");
}

void setup()
{
  Serial.begin(9600);
  //lcd.begin(16, 2); //prosedur pemanggilan LCD
  //lcd.setBacklight(255);
  //
  //lcd.setCursor(00, 00); //set pada baris 1 dan kolom 1
  //lcd.print("*Hello World*"); //menuliskan "Hello World"
  //lcd.setCursor(00, 1); //set pada baris 2 dan kolom 1
  //lcd.print("*nyebarilmu.com*"); //menuliskan "nyebarilmu.com"
  //
  //lcd.clear(); //menghapus data sebelumnya
  //
  //delay(3000);
  
  //calib - Cur Const= Ratio/BurdenR. 2000/33 = 60
  emon1.current(pin_sct, 17);   //15); //21); //40);  //10);      //60);


    initLfs();
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  //mencetak acces point
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);//"text/html"); 
  });

  
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
   
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  

//  // Route to load style.css file
//  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(SPIFFS, "/style.css", "text/css");
//  });
//  

  //agar html bisa membaca file style css yang filenya terpisah
  server.serveStatic("/",SPIFFS,"/");
  // Start server
  server.begin();
}

void loop()
{ 
  double Irms = emon1.calcIrms(1480);
  //menampilkan di serial monitor
  Serial.print("Arus yang terbaca : ");
  Serial.print(Irms);
  Serial.print("         ");
  Serial.print("Daya yang terbaca : ");
  Serial.println(Irms*tegangan);
  aruss=Irms;
  dayaa=Irms*tegangan;


/*
  //menampilkan di LCD 16x2
  lcd.setCursor(00,00);
  lcd.print("Arus (A):");
  lcd.setCursor(00,1);
  lcd.print("Daya (W):"); 

//Arus
  lcd.setCursor(10,00);
  lcd.print(Irms);
 
//Daya
  lcd.setCursor(10,1);
  lcd.print("      ");
  lcd.setCursor(10,1);
  lcd.print(Irms*tegangan,1);
  */
  
  delay(700);
}
