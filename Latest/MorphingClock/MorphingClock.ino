#include <PxMatrix.h>
#include <Adafruit_GFX.h>
#include "Digit.h"
#include <NTPClient.h>
#include <TimeLib.h>


#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager
char wifiManagerAPName[] = "MorphClk";
char wifiManagerAPPassword[] = "HariFun";

WiFiUDP udp; 
unsigned int localPort = 2390;  
NTPClient timeClient(udp, "nl.pool.ntp.org", 1, 60*60*1000);

int brightness = 50;

//Scrolltext 
#include <Fonts/kongtext4pt7b.h>
#define SCROLLYPOS 9 //y position of the scrolltext
#define CHARACTERWIDTH 8 //width of the characters in the font
int scrollXpos=64; //current x position of the scrolltext

#include <Fonts/TomThumb.h>

uint16_t EdgeColor;
uint16_t HourColor = 65504;
uint16_t MinuteColor = 65504;
uint16_t SecondsColor = 31;
uint16_t ScrollColor = 63488;
String ScrollText ="...";

//NTPClient ntpClient;
byte prevhh;
byte prevmm;
byte prevss;

//timer variables
int MORPHDELAY = 50;
int SCROLLTEXTDELAY = 5;
long checkmorphTimer = 0;
long lastcheckedtime = 0;
long scrolltextTimer = 0;
long TimeUpdaterTimer = 0;
bool UpdateTime=true;
String maand[] = {"januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december"};
String maandkort[] = {"jan", "feb", "mrt", "apr", "mei", "jun", "jul", "aug", "sep", "okt", "nov", "dec"};
String dag[] = {"zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag"};
String dagkort[] = {"zo", "ma", "di", "wo", "do", "vr", "za"};

#ifdef ESP32
  #define P_LAT 22
  #define P_A 19
  #define P_B 23
  #define P_C 18
  #define P_D 5
  #define P_E 15
  #define P_OE 2
  hw_timer_t * timer = NULL;
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


#endif

#ifdef ESP8266
  #include <Ticker.h>
  Ticker display_ticker;
  #define P_LAT 16
  #define P_A 5
  #define P_B 4
  #define P_C 15
  #define P_D 12
  #define P_E 0
  #define P_OE 2
  
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <FS.h>
  
#endif

AsyncWebServer server(80);
DNSServer dns;
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

#include "webrequests.h"

#ifdef ESP8266
  // ISR for display refresh
  void display_updater()
  {
    //display.displayTestPattern(70);
    display.display(brightness);
  }
#endif

#ifdef ESP32
  void IRAM_ATTR display_updater() {
    // Increment the counter and set the time of ISR
    portENTER_CRITICAL_ISR(&timerMux);
    //isplay.display(70);
    display.displayTestPattern(70);
    portEXIT_CRITICAL_ISR(&timerMux);
  }
#endif

Digit digit0(&display, 0, 68 - 1 - 15*1, 3, SecondsColor,3,3);  //sS
Digit digit1(&display, 0, 68 - 1 - 21, 3, SecondsColor,3,3);    //Ss
Digit digit2(&display, 0, 68 - 4 - 9*3, 3, MinuteColor,6,6); //mM
Digit digit3(&display, 0, 68 - 4 - 9*4, 3, MinuteColor,6,6); //Mm
Digit digit4(&display, 0, 68 - 7 - 9*5, 3, HourColor,6,6); //hH
Digit digit5(&display, 0, 68 - 7 - 9*6, 3, HourColor,6,6); //Hh


//Check if a digit is in state doMorph()
void checkMorph(){
  if (millis()-checkmorphTimer>MORPHDELAY)
  {
    Serial.print("checkmorphTimer: ");
    Serial.print(millis()-checkmorphTimer);
        
    if (digit0.doMorph()) digit0.MorphDigit();
    if (digit1.doMorph()) digit1.MorphDigit();
    if (digit2.doMorph()) digit2.MorphDigit();
    if (digit3.doMorph()) digit3.MorphDigit();
    if (digit4.doMorph()) digit4.MorphDigit();
    if (digit5.doMorph()) digit5.MorphDigit();
    checkmorphTimer = millis();
  }
}

void connectWifi()
{
   digitalWrite(LED_BUILTIN, HIGH);
   display.setCursor(2, 12);
   display.print("Connecting");
   //fetches ssid and pass from eeprom and tries to connect
   //if it does not connect it starts an access point with the specified name wifiManagerAPName
   //and goes into a blocking loop awaiting configuration

   AsyncWiFiManager wifiManager(&server,&dns);;
   wifiManager.autoConnect(wifiManagerAPName, wifiManagerAPPassword);

  //-- Status --
  display.fillScreen(display.color565(0, 0, 0));
  display.setCursor(2, 2);
  display.print("Connected!");
  display.setCursor(0, 12);
  display.setFont(&TomThumb);
  display.print(WiFi.localIP());
  Serial.println("WiFi connected");

  Serial.println("IP address: ");
  display.setFont();
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  //delay(5000);
}

void webserverSetup(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?scrolltext=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET scrolltext value on <ESP_IP>/get?scrolltext=<inputMessage>
    if (request->hasParam("scrolltext")) {
      ScrollText = request->getParam("scrolltext")->value();
      writeFile(SPIFFS, "/scrolltext.txt", ScrollText.c_str());
      formatScrollText();
    }
    else if (request->hasParam("HourColor")) {
      inputMessage = request->getParam("HourColor")->value();
      writeFile(SPIFFS, "/HourColor.txt", inputMessage.c_str());
      HourColor = inputMessage.toInt();
      digit5.setColorDigit(HourColor);
      digit4.setColorDigit(HourColor);
      //drawTime();
    }    
    else if (request->hasParam("MinuteColor")) {
      inputMessage = request->getParam("MinuteColor")->value();
      writeFile(SPIFFS, "/MinuteColor.txt", inputMessage.c_str());
      MinuteColor = inputMessage.toInt();
      digit3.setColorDigit(MinuteColor);
      digit2.setColorDigit(MinuteColor);        
      //drawTime();
    }
    else if (request->hasParam("SecondsColor")) {
      inputMessage = request->getParam("SecondsColor")->value();
      writeFile(SPIFFS, "/SecondsColor.txt", inputMessage.c_str());
      SecondsColor = inputMessage.toInt();
      digit1.setColorDigit(SecondsColor);
      digit0.setColorDigit(SecondsColor);
      //drawTime();
    }    
    else if (request->hasParam("EdgeColor")) {
      inputMessage = request->getParam("EdgeColor")->value();
      writeFile(SPIFFS, "/EdgeColor.txt", inputMessage.c_str());
      EdgeColor = inputMessage.toInt();
    }    
    else if (request->hasParam("ScrollColor")) {
      inputMessage = request->getParam("ScrollColor")->value();
      writeFile(SPIFFS, "/ScrollColor.txt", inputMessage.c_str());
      ScrollColor = inputMessage.toInt();
    }    
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();  
}

void setup() {
  Serial.begin(9600);
  #ifdef ESP32
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif
// Send web page with input fields to client
  webserverSetup();

  display.begin(16);
  display.setFastUpdate(true);
  display.setRotation(0); 
  #ifdef ESP8266
    display_ticker.attach(0.002, display_updater);
  #endif
  #ifdef ESP32
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 2000, true);
    timerAlarmEnable(timer);
  #endif
  //connect to Wifi
  connectWifi();
  //start timeclient
  timeClient.begin();
  UpdateTime = true;
  timeUpdate();
  readData();
  formatScrollText();
  //settime();

}

void drawOverlay()
{
  display.drawRect(0, 0, 64, 32, EdgeColor);
}


void scroll_text()
{ 
  if(millis()-scrolltextTimer>SCROLLTEXTDELAY)
  {
    Serial.print("scroll_text: ");
    Serial.print(millis()-scrolltextTimer);
    
    display.setTextWrap(false); 
    display.setFont(&kongtext4pt7b);
    display.setTextColor(ScrollColor);
    display.setCursor(scrollXpos--,SCROLLYPOS);
    display.fillRect(1, 1, 64, 10, display.color565(0, 0, 0)); //empty the scroll text bar
    display.println(ScrollText);
    int minpos=0-(ScrollText.length()*CHARACTERWIDTH);
    if (scrollXpos<minpos) scrollXpos=64; //reset xposition when text scrolled to left.
    display.setFont();
    drawOverlay();
    scrolltextTimer = millis();
  }
}

void updateDigits()
{
  //update digit values every second
  if(millis()-lastcheckedtime>1000)
  {
    Serial.print("updateDigits: ");
    Serial.print(millis()-lastcheckedtime);
    
    int ss = second();
    int mm = minute();
    int hh = hour();
    if (ss!=prevss) { 
      int s0 = ss % 10;
      int s1 = ss / 10;
      if (s0!=digit0.Value()) digit0.setValue(s0);
      if (s1!=digit1.Value()) digit1.setValue(s1);
      //ntpClient.PrintTime();
      prevss = ss;
    }
   if (mm!=prevmm) {
      int m0 = mm % 10;
      int m1 = mm / 10;
      if (m0!=digit2.Value()) digit2.setValue(m0);
      if (m1!=digit3.Value()) digit3.setValue(m1);
      prevmm = mm;
    }
    
    if (hh!=prevhh) {
      int h0 = hh % 10;
      int h1 = hh / 10;
      if (h0!=digit4.Value()) digit4.setValue(h0);
      if (h1!=digit5.Value()) digit5.setValue(h1);
      prevhh = hh;
    }  
  Serial.println (timeClient.getFormattedTime()); 
  lastcheckedtime=millis();
  }

}

//sets the time received from NTC into Time object
void settime()
{
  display.fillScreen(display.color565(0, 0, 0));
  if (hour()==0 && (minute()-26)==0 && second()==0) ESP.restart();
  String date_time = timeClient.getFormattedDate();
  int iyear = date_time.substring(0,4).toInt();
  int imonth = date_time.substring(6,7).toInt();
  int iday = date_time.substring(9,10).toInt();
  setTime(timeClient.getHours()+2,timeClient.getMinutes(),timeClient.getSeconds(),iday, imonth, iyear);
  Serial.println(date_time);
  Serial.println(date_time.substring(0,4));
  Serial.println(date_time.substring(6,7));
  Serial.println(date_time.substring(9,10));

  drawTime();
}

void drawTime(){
  int hh = hour();
  int mm = minute();
  int ss = second();
  digit0.Draw(ss % 10);
  digit1.Draw(ss / 10);
  digit2.Draw(mm % 10);
  digit3.Draw(mm / 10);
  digit4.Draw(hh % 10);
  if (hh>10){
    digit5.Draw(hh / 10);
  }
  //draw Colon after digit 3 (hH)
  digit3.DrawColon(display.color565(255, 255, 0));
}
void timeUpdate(){
  if ((millis()-TimeUpdaterTimer>3600000)||UpdateTime)
  {
    Serial.print("Timeupdater: ");
    Serial.print(millis()-TimeUpdaterTimer);
    UpdateTime = !timeClient.update();
    Serial.println(UpdateTime);
    TimeUpdaterTimer = millis();
    if (!UpdateTime) {
      settime();
    }
  }
}
void readData(){
    ScrollText = readFile(SPIFFS, "/scrolltext.txt");
    HourColor = readFile(SPIFFS, "/HourColor.txt").toInt();
    MinuteColor = readFile(SPIFFS, "/MinuteColor.txt").toInt();
    SecondsColor = readFile(SPIFFS, "/SecondsColor.txt").toInt();
    EdgeColor = readFile(SPIFFS, "/EdgeColor.txt").toInt();
    ScrollColor = readFile(SPIFFS, "/ScrollColor.txt").toInt();

}

void formatScrollText()
{
  if(ScrollText.indexOf("[datum]")>=0){
    ScrollText.replace("[datum]", String(day()) + " " + maand[month()-1] + " " +  String(year()));
  }
  if(ScrollText.indexOf("[MMMM]")>=0){
    ScrollText.replace("[MMMM]", maand[month()-1]);
  }
  if(ScrollText.indexOf("[MMM]")>=0){
    ScrollText.replace("[MMM]", maandkort[month()-1]);
  }
  if(ScrollText.indexOf("[DDDD]")>=0){
    ScrollText.replace("[DDDD]", dag[day()-1]);
  }
  if(ScrollText.indexOf("[DDD]")>=0){
    ScrollText.replace("[DDD]", dagkort[day()-1]);
  }
  if(ScrollText.indexOf("[groet]")>=0){
    if(hour()>=18){
       ScrollText.replace("[groet]", "Goedenavond");      
    }   
    if(hour()>=12){
       ScrollText.replace("[groet]", "Goedemiddag");      
    }      
    if(hour()>=6){
       ScrollText.replace("[groet]", "Goedemorgen");      
    }
    if(hour()>=0){
       ScrollText.replace("[groet]", "Goedenacht");      
    }
  }
}

void loop() {
  timeUpdate();
  checkMorph();
  updateDigits();
  //readData();
  scroll_text();
}
