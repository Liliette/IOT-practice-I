#include <SPI.h>
#include <WiFi.h>
#include <LWiFi.h>
#include "DHT.h"
#define dht_dpinD11
#define DHTPIN 11     // what pin we're connected to
#define DHTTYPE DHT11

const int buttonPin = 2;
const int buttonPin_b = 3;//blue led
const int relayPin = 13;
const int ledPin = 12;
int relayState = 1;
bool buttonState_b;
bool ledState = HIGH;

//wifi
char ssid[] = "220-1";      //  your network SSID (name)
char pass[] = "ntuedorm";   // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);
String webSite;

//dht
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin_b, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

//wifi
    Serial.begin(9600);
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  Serial.begin(9600);

  //buildWeb();
  printWifiStatus();                        // you're connected now, so print out the status
  
  dht.begin();
  Serial.print("Test humidity and temperature");
}

/*void buildWeb(){
  webSite += "<html>";
  webSite += "<head>\n";
  webSite += "<mata charst='UTF-8'/>\n";
  webSite += "<title> IOT </title>\n";
  webSite += "<style>";
  webSite += "html{background-color:#FAFCD6; margin: auto; font-family: 'Arial';}";
  webSite += ".title{text-align:center;color: #1E6B7F;font-size: 3em;}";
  webSite += ".div1{background-color: #40A798; max-width:500px; margin:10px auto;padding: 10px;";
  webSite += "border-radius: 25px; color: white; text-align: center;font-size: 2em;}";
  webSite += "div{background-color: #FCE38A; max-width:500px; margin:10px auto;padding: 10px;";
  webSite += "border-radius: 25px;}";
  webSite += "h2{color:#F86254; text-align: center; font-size: 2em;}";
  webSite += "a{display: block;text-decoration: none; background-color:#F86254;color: white;padding: 10px 20px;";
  webSite += "width:150px;text-align:center;margin:0 auto;margin-bottom:30px;fony-size:2em; }";
  webSite += "";
  webSite += "</style>";
  webSite += "</head>\n";
  webSite += "<body>";
  webSite += "<h1 class=\"title\">Smart your life</h1>";
  webSite += "<div class=\"div1\">";
  webSite += "<p>T=22</p>";
  webSite += "<p>H=50%</p>";
  webSite += "</div>";
  webSite += "<div>";
  webSite += "<h2>FAN</h2>";
  webSite += "<a href=\"/O\">ON</a>";
  webSite += "<a href=\"/F\">OFF</a>";
  webSite += "</div>";
  webSite += "<div>";
  webSite += "<h2>LIGHT</h2>";
  webSite += "<a href=\"/H\">ON</a>";
  webSite += "<a href=\"/L\">OFF</a>";
  webSite += "</div>";
  webSite += "</body></html>";
  }*/

void switchRelay()
{
  if(relayState == 0)
    relayState = 1;
  else
    relayState = 0;

  digitalWrite(relayPin, relayState);
  Serial.print("Relay state: ");
  Serial.println(relayState);
  }

  void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
  
void loop() {
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  String S[2]={"off","on"};

  int buttonState;
  bool l=ledState;
  bool r=relayState;
  
  buttonState = digitalRead(buttonPin);
  buttonState_b = digitalRead(buttonPin_b);

  
  if(buttonState_b == HIGH){
    ledState=!ledState;
    l=ledState;
    digitalWrite(ledPin, ledState);
    delay(20);
    while(1){
      buttonState_b=digitalRead(buttonPin_b);
      if(buttonState_b==HIGH) {
        l=ledState;
        break;
      }
    }
    delay(20);
    }
    
    if(buttonState == HIGH){
    switchRelay();
    r=relayState;
    delay(500);
    }
    
    Serial.print("Humidity = ");
    Serial.print(h);
    Serial.print("% ");
    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.println("C ");
    delay(1000);

    WiFiClient client = server.available();   // listen for incoming clients
    if (client) {                             // if you get a client,
      Serial.println("new client");           // print a message out the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          if (c == '\n') {                    // if the byte is a newline character
  
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Refresh:5");
              client.println("<html>");
              client.println("<head>\n");
              client.println("<mata charst='UTF-8'/>\n");
              client.println("<title> IOT </title>\n");
              client.println("<style>");
              client.println("html{background-color:#FAFCD6; margin: auto; font-family: 'Arial';}");
              client.println(".title{text-align:center;color: #1E6B7F;font-size: 3em;}");
              client.println(".div1{background-color: #40A798; max-width:500px; margin:10px auto;padding: 10px; border-radius: 25px; color: white; text-align: center;font-size: 2em;}");
              client.println("div{background-color: #FCE38A; max-width:500px; margin:10px auto;padding: 10px; border-radius: 25px;}");
              client.println("h2{color:#F86254; text-align: center; font-size: 2em;}");
              client.println("a{display: block;text-decoration: none; background-color:#F86254; color: white; padding: 10px 20px;width:150px;text-align:center;margin:0 auto;margin-bottom:30px;fony-size:2em; }");
              client.println("</style>");
              client.println("</head>\n");
              client.println("<body>");
              client.println("<h1 class=\"title\">Smart your life</h1>");
              client.println("<div class=\"div1\">");
              client.println("<p>T=");
              client.println(t);
              client.println("*</p>");
              client.println("<p>H=");
              client.println(h);
              client.println("%</p>");
              client.println("</div>");
              client.println("<div>");
              client.println("<h2>FAN ");
              client.println(S[r]);
              client.println("</h2>");
              client.println("<a href=\"/O\">ON</a>");
              client.println("<a href=\"/F\">OFF</a>");
              client.println("</div>");
              client.println("<div>");
              client.println("<h2>LIGHT  ");
              client.println(S[l]);
              client.println("</h2>");
              client.println("<a href=\"/H\">ON</a>");
              client.println("<a href=\"/L\">OFF</a>");
              client.println("</div>");
              client.println("</body></html>");
                           
              // the content of the HTTP response follows the header:
              //client.print(webSite);
              
  
              // The HTTP response ends with another blank line:
              client.println();
              // break out of the while loop:
              break;
            } else {    // if you got a newline, then clear currentLine:
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }
  
          // Check to see if the client request was "GET /H" or "GET /L":
          if (currentLine.startsWith("GET /H")) {
            digitalWrite(ledPin, HIGH);               // GET /H turns the LED on
            ledState=HIGH;
            l=ledState;
            //buttonState_b = LOW;
            //break;
          }
          if (currentLine.startsWith("GET /L")) {
            digitalWrite(ledPin, LOW);                // GET /L turns the LED off
            ledState=LOW;
            l=ledState;
            //buttonState_b = HIGH;
            //break;
          }

          if(currentLine.startsWith("GET /F")){
              /*if(buttonState == LOW){
              switchRelay();
              delay(500);
              }*/
              /*if(relayState == 0)
              relayState=1;
              delay(500);*/
              buttonState =LOW;
              if(buttonState == LOW){
              relayState=1;
              r=!relayState;
              delay(500);
              digitalWrite(relayPin, relayState);
              }              
            }
           if(currentLine.startsWith("GET /O")){
             /* if(buttonState == HIGH){
              switchRelay();
              delay(500);
              }*/
              /*if(relayState==1)
              relayState=0;
              delay(500);*/
              buttonState = HIGH;
              if(buttonState == HIGH){
                relayState=0;
                r=!relayState;
                delay(500);
                digitalWrite(relayPin, relayState);
              }
            }
        }
      }
      // close the connection:
      client.stop();
      Serial.println("client disonnected");
    }
    
}
