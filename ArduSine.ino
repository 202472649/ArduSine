#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "NOAHLAPTOP";      //WIFI NAME
const char* password = "61!3Vf05B3";  //WIFI PASSWORD

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL)
// On ESP8266:              D2(SDA), D1(SCL)
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define GREEN_LED 12
#define BLUE_LED 14
#define RED_LED 13

int testValue = 69;
float amplitude = 1;
float largeur = 1;
float offsetX = 0;
float offsetY = 0;

AsyncWebServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("OLED SSD1306 allocation failed"));
  }
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..."); 
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Connectingto WiFi...");
    display.setTextSize(1);
    display.println();
    display.print("SSID : ");
    display.println(ssid);
    display.display();
    digitalWrite(RED_LED, HIGH);
  }
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Connected to WiFi");
  display.println();
  display.setTextSize(1);
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
  delay(8000);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  defineHTMLRoutes(); //Fonction to call Routes to HTML Page bellow
  server.begin();  //Start the WEB Server
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("y = a*sin(b(x-h))+k");
  display.println("a = " + (String)amplitude);
  display.println("b = " + (String)largeur);
  display.println("h = " + (String)offsetX);
  display.println("k = " + (String)offsetY);
  display.display();
  delay(1000);
}

void defineHTMLRoutes() {
  // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", "<html>\
    <head>\
      <style>\
        body {\
          font-family : Arial, sans-serif;\
          margin:0;padding:0;\
          display:flex;\
          justify-content : center;\
          align-items : center;\
          height:100vh;\
          background-color : #f0f0f0;\
        }\
        .container {\
          text-align : center;\
          width:80% ;\
          max-width : 800px;\
          background-color : #fff;\
          padding:20px;\
          margin:20px;\
          border-radius : 10px;\
          box-shadow : 0 0 10px rgba(0, 0, 0, 0.1);\
        }\
        header {\
          background-color : #3498db;\
          color:white;\
          padding:20px;\
          border-radius : 10px;\
        }\
        h1 {\
          margin:0;\
        }\
        h3 {\
          margin:0;\
        }\
        a:link, a:visited {\
          background-color: #e56e06;\
          color: white;\
          padding: 10px 20px;\
          text-align: center;\
          text-decoration: none;\
          display: inline-block;\
          border-radius : 20px;\
          margin: 20px 5px 10px 5px;\
        }\
        a:hover, a:active {\
          background-color: #B05607;\
        }\
      </style>\
    </head>\
    <body>\
      <div class=\"container\">\
        <header>\
          <h1>PFI Maths Electro Hiver 2025</h1>\
          <h3>Par Noah Bleau et Tristan Houle</h3>\
          <a href=\"/graph-plotter\">Graph Plotter</a>\
          <a href=\"/data-viewer\">Data Viewer</a>\
        </header>\
      </div>\
    </body>\
    </html>");
  });

  server.on("/data-viewer", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", "<html>\
    <head>\
      <meta http-equiv=\"refresh\" content=\"1\">\
      <title>Data Viewer</title>\
      <style>\
        body {\
          font-family : Arial, sans-serif;\
          margin:0;padding:0;\
          display:flex;\
          justify-content : center;\
          align-items : center;\
          height:100vh;\
          background-color : #f0f0f0;\
        }\
        .container {\
          text-align : center;\
          width:50% ;\
          max-width : 500px;\
          background-color : #fff;\
          padding:20px;\
          margin:20px;\
          border-radius : 10px;\
          box-shadow : 0 0 10px rgba(0, 0, 0, 0.1);\
        }\
        header {\
          background-color : #3498db;\
          color:white;\
          padding:20px;\
          border-radius : 10px;\
          align-items : center;\
        }\
        h1 {\
          margin:0;\
        }\
        h3 {\
          margin:0;\
        }\
        .sub-container {\
          background-color : #fff;\
          color:black;\
          padding:10px;\
          margin:20px 100px 10px 100px;\
          border-radius : 10px;\
          box-shadow : 0 0 10px rgba(0, 0, 0, 0.1);\
        }\
      </style>\
    </head>\
    <body>\
      <div class=\"container\">\
        <header>\
          <h1>Data Viewer</h1>\
          <a href=\"/\">Return to Home Page</a>\
          <div class=\"sub-container\">\
            <p>testValue = " + (String)testValue + "</p>\
            <p>Amplitude (a) = " + (String)amplitude + "</p>\
            <p>Largeur (b) = " + (String)largeur + "</p>\
            <p>Offset X (h) = " + (String)offsetX + "</p>\
            <p>Offset Y (k) = " + (String)offsetY + "</p>\
          </div>\
        </header>\
      </div>\
    </body>\
    </html>");
  });

  server.on("/graph-plotter", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", "<html>\
    <head>\
      <script src=\"https://cdn.plot.ly/plotly-3.0.1.min.js\"></script>\
      <script src=\"https://cdnjs.cloudflare.com/ajax/libs/mathjs/9.5.0/math.js\"></script>\
      <title>Graph Plotter</title>\
      <script>\
          window.onload = function(){\
            const graph = document.getElementById('graph');\
            graph.innerHTML = \'\';\
            const expr = math.parse(\"y= " + (String)amplitude + "*sin(" + (String)largeur + "*(x-" + (String)offsetX + ")) + " + (String)offsetY + "\").compile();\
            const xValues = math.range(-10, 10, 0.1)._data;\
            const yValues = xValues.map(x => expr.evaluate({ x }));\
            const trace = {x: xValues,y: yValues,type: \'scatter',mode: \'lines\',};\
            const layout = {xaxis: { title: \'X-axis\' },yaxis: { title: \'Y-axis\' },};\
            Plotly.newPlot(graph, [trace], layout);\
          }\
      </script>\
      <style>\
        body {\
          font-family : Arial, sans-serif;\
          margin:0;padding:0;\
          display:flex;\
          justify-content : center;\
          align-items : center;\
          height:100vh;\
          background-color : #f0f0f0;\
        }\
        .container {\
          text-align : center;\
          width:80% ;\
          max-width : 800px;\
          background-color : #fff;\
          padding:20px;\
          margin:20px;\
          border-radius : 10px;\
          box-shadow : 0 0 10px rgba(0, 0, 0, 0.1);\
        }\
        header {\
          background-color : #3498db;\
          color:white;\
          padding:20px;\
          border-radius : 10px;\
        }\
        h1 {\
          margin:0;\
        }\
        .main-content {\
          display:flex;\
          flex-direction : column;\
          align-items : center;\
          margin-top : 5px;\
          padding-top : 5px;\
        }\
        .graph {\
          width:100% ;\
          height:400px;\
          margin:20px 0;\
          border:1px solid #ccc;\
          border-radius : 10px;\
        }\
      </style>\
    </head>\
    <body>\
      <div class=\"container\">\
        <header>\
          <h1>Graph Plotter</h1>\
          <a href=\"/\">Return to Home Page</a>\
        </header>\
        <div class=\"main-content\">\
          <div class=\"graph\" id=\"graph\">\
          </div>\
        </div>\
      </div>\
    </body>\
    </html>");
  });
}