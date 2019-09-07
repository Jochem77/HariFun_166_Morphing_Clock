// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
// https://randomnerdtutorials.com/esp32-esp8266-input-data-html-form/

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    scrolltext: <input type="text" name="scrolltext" value="%scrolltext%">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <form action="/get" target="hidden-form">
      hour color: 
    <input type="text" name="HourColor" style="width: 68px" value="%HourColor%">&nbsp;&nbsp;
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <form action="/get" target="hidden-form">
      minute color: 
    <input type="text" name="MinuteColor" style="width: 68px" value="%MinuteColor%">&nbsp;&nbsp;
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>  
  <form action="/get" target="hidden-form">
      second color: 
    <input type="text" name="SecondsColor" style="width: 68px" value="%SecondsColor%">&nbsp;&nbsp;
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>  
  <form action="/get" target="hidden-form">
      edge color: 
    <input type="text" name="EdgeColor" style="width: 68px" value="%EdgeColor%">&nbsp;&nbsp;
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>  
  <form action="/get" target="hidden-form">
      scroll color: 
    <input type="text" name="ScrollColor" style="width: 68px" value="%ScrollColor%">&nbsp;&nbsp;
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>  
  <br>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  //Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "scrolltext"){
    return readFile(SPIFFS, "/scrolltext.txt");
  }
  if(var == "HourColor"){
    return readFile(SPIFFS, "/HourColor.txt");
  }
  if(var == "MinuteColor"){
    return readFile(SPIFFS, "/MinuteColor.txt");
  }  
  if(var == "SecondsColor"){
    return readFile(SPIFFS, "/SecondsColor.txt");
  }  
  if(var == "ScrollColor"){
    return readFile(SPIFFS, "/ScrollColor.txt");
  }  
  if(var == "EdgeColor"){
    return readFile(SPIFFS, "/EdgeColor.txt");
  }  
  return String();
}
