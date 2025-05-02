#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ############################################################ //
// ### J'UTILISE UN ESP8266, DONC LES PINS SONT DIFFÉRENTES ### //
// ############################################################ //

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
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Define the display

#define GREEN_LED 12 //Pin D6
#define BLUE_LED 14 //Pin D5
#define RED_LED 13 //Pin D7

#define selectorPin ADC0 //Pin A0 : Potentiometer
#define leftButton 3 //Pin RX
#define rightButton 1 //Pin TX

//Define the function mode here
String functionMode = "sin"; //Options : sin, cos, tan, abs, log, exp, aff, quad, para

//Define the parameters for all functions
float a, b, h, k = 0;

//Define the variable to store the function
String functionFinal = "y=x*2";

AsyncWebServer server(80); //Configure web server on port 80 (HTTP)

void setup() {
  Serial.begin(9600); //Initialize Serial Monitor with baud 9600
  pinMode(GREEN_LED, OUTPUT); //Define LED as OUTPUT
  pinMode(BLUE_LED, OUTPUT); //Define LED as OUTPUT
  pinMode(RED_LED, OUTPUT); //Define LED as OUTPUT

  //Warn in the Serial if the OLED Display failed to start
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //If the display not initialized
    Serial.println(F("OLED SSD1306 allocation failed")); //Print text to Serial Monitor
  }

  // Startup display configuration
  displayClear(); //Clear the display
  displayShow(); //Prevent error on startup. Make sure nothing is on screen
  displayColor("WHITE"); //Set the diplay color as WHITE

  // Connect to Wi-Fi
  WiFi.begin(ssid, password); //Connect to the Wifi
  while (WiFi.status() != WL_CONNECTED) { //Waiting for WiFi to connect
    delay(1000); //Wait 1 seconds
    wifiConnecting(); //Script when WiFi is connecting. Show text in Serial and on the OLED
    ledRED(); //Set the LED to RED
  }
  ledGREEN(); //Set the LED to GREEN
  wifiConnected(); //Script when WiFi is connected. Show text in Serial and on the OLED
  delay(8000); //Wait 8 seconds

  //Start the Web Server
  defineHTMLRoutes(); //Fonction to call Routes to HTML Page bellow
  server.begin();  //Start the WEB Server
  ledBLUE(); //Set the LED to BLUE
  displayPrintTextLargeFull("Check Serial Monitor"); //Print text on OLED

  //Ask for the function mode
  Serial.println("Please input the desired function. Here are the available options : "); //Ask Question
  Serial.println("sin, cos, tan, abs, exp, quad, affine"); //List available options
  while(!Serial.available); //Wait for user input
  functionMode = Serial.readString(); //Convert the Serial input to char in functionMode
  if(functionMode == "sin"){ //the input was 'sin'
    Serial.println("You have selected the Sinus function! For this program we use the canonical form : f(x) = a*sin(b(x-h)) + k"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'b'? (ex.: 6.37)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b
    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h
    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k
    Serial.println("Here is your selection : a = " + String(a) + ", b = " + String(b) + ", h = " + String(h) + ", k = " + String(k)); //Confirm user choice
  } else if(functionMode == "cos"){ //the input was 'cos'
    Serial.println("You have selected the Cosinus function! For this program we use the canonical form : f(x) = a*cos(b(x-h)) + k"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'b'? (ex.: 6.37)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b
    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h
    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k
    Serial.println("Here is your selection : a = " + String(a) + ", b = " + String(b) + ", h = " + String(h) + ", k = " + String(k)); //Confirm user choice
  } else if(functionMode == "tan"){ //the input was 'tan'
    Serial.println("You have selected the Tangent function! For this program we use the canonical form : f(x) = a*tan(b(x-h)) + k"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'b'? (ex.: 6.37)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b
    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h
    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k
    Serial.println("Here is your selection : a = " + String(a) + ", b = " + String(b) + ", h = " + String(h) + ", k = " + String(k)); //Confirm user choice
  } else if(functionMode == "abs"){ //the input was 'abs'
    Serial.println("You have selected the Absolute function! For this program we use the canonical form : f(x) = a*abs(x-h) + k"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h
    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k
    Serial.println("Here is your selection : a = " + String(a) + ", h = " + String(h) + ", k = " + String(k)); //Confirm user choice
  } else if(functionMode == "exp"){ //the input was 'exp'
    Serial.println("You have selected the Exponential function! For this program we use the canonical form : f(x) = a*exp(b(x-h)) + k"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'b'? (ex.: 6.37)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b
    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h
    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k
    Serial.println("Here is your selection : a = " + String(a) + ", b = " + String(b) + ", h = " + String(h) + ", k = " + String(k)); //Confirm user choice
  } else if(functionMode == "quad"){ //the input was 'quad'
    Serial.println("You have selected the Quadratic function! For this program we use the canonical form : f(x) = a*((x-h)*exp(2)) + k"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h
    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k
    Serial.println("Here is your selection : a = " + String(a) + ", h = " + String(h) + ", k = " + String(k)); //Confirm user choice
  } else if(functionMode == "affine"){ //the input was 'affine'
    Serial.println("You have selected the Affine function! For this program we use the base form : f(x) = a*x + b"); //Confirm user choice
    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a
    Serial.println("What is 'b'? (ex.: 6.37)"); //Ask user for is choice
    while(!Serial.available); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b
    Serial.println("Here is your selection : a = " + String(a) + ", b = " + String(b)); //Confirm user choice
  } else { //the input was invalid, the user typed something not in the options
    Serial.println("\"" + String(functionMode) + "\" is an invalid mode! Program Stopped."); //Tell the user the error
    displayClear(); //Clear the display
    displayShow(); //Show the modification on the display
    while(true); //Make an infinite loop to 'stop' the program
  }
  delay(5000); //Wait 5 seconds
}

void loop() {
  
  
}

// ############################### //
// ### OTHERS FUNCTIONS BELLOW ### //
// ############################### //

//Clear the display
void displayClear(){
  display.clearDisplay();
}

//Show the modification on the display
void displayShow(){
  display.display();
}

//Set the Cursor placement on the display
void displayCursor(int x, int y){
  display.setCursor(x,y);
}

//Set the display color
void displayColor(char color){
  display.setTextColor(color);
}

//Print the input text in large 
void displayPrintTextLarge(String text){
  display.setTextSize(2);
  display.println(text);
}

//Print the input text in small
void displayPrintTextSmall(String text){
  display.setTextSize(1);
  display.println(text);
}

//Print the input text in large with clear and show
void displayPrintTextLargeFull(String text){
  displayClear();
  displayCursor(0, 0);
  displayPrintTextLarge(text);
  displayShow();
}

//Print the input text in small with clear and show
void displayPrintTextSmallFull(String text){
  displayClear();
  displayCursor(0, 0);
  displayPrintTextSmall(text);
  displayShow();
}

//Set RED led on and others off
void ledRED(){
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

//Set BLUE led on and others off
void ledBLUE(){
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(RED_LED, LOW);
}

//Set GREEN led on and others off
void ledGREEN(){
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

//DO NOT REUSE
void wifiConnected(){
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
}

//DO NOT REUSE
void wifiConnecting(){
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
}

//DO NOT REUSE
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
            const expr = math.parse(\"" + functionFinal + "\").compile();\
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