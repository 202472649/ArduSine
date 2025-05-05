#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ############################################################ //
// ### J'UTILISE UN ESP8266, DONC LES PINS SONT DIFFÉRENTES ### //
// ############################################################ //

// THESE TWO NEEDS TO BE CHANGED ACCORDING TO YOURS 
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
char selectedParameter = "a";

//Incrementation range for the potentiometer
int incrementationRange = 0;
int mappedIncrementationValue = 0;

//Define the variable to store the function
String functionFinal = "y=0";

AsyncWebServer server(80); //Configure web server on port 80 (HTTP)

// ################## //
// ### MAIN SETUP ### //
// ################## //

void setup() {
  Serial.begin(9600); //Initialize Serial Monitor with baud 9600
  pinMode(GREEN_LED, OUTPUT); //Define LED as OUTPUT
  pinMode(BLUE_LED, OUTPUT); //Define LED as OUTPUT
  pinMode(RED_LED, OUTPUT); //Define LED as OUTPUT
  pinMode(leftButton, INPUT_PULLUP); //Define BUTTON as INPUT with internal resistor
  pinMode(rightButton, INPUT_PULLUP); //Define BUTTON as INPUT with internal resistor

  //Warn in the Serial if the OLED Display failed to start
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //If the display not initialized
    Serial.println(F("OLED SSD1306 allocation failed")); //Print text to Serial Monitor
  }

  // Startup display configuration
  displayClear(); //Clear the display
  displayShow(); //Prevent error on startup. Make sure nothing is on screen
  display.setTextColor(WHITE); //Set the diplay color as WHITE

  // Connect to Wi-Fi
  WiFi.begin(ssid, password); //Connect to the Wifi
  while (WiFi.status() != WL_CONNECTED) { //Waiting for WiFi to connect
    delay(1000); //Wait 1 seconds
    wifiConnecting(); //Script when WiFi is connecting. Show text in Serial and on the OLED (DO NOT REUSE AGAIN)
    ledRED(); //Set the LED to RED
  }
  ledGREEN(); //Set the LED to GREEN
  wifiConnected(); //Script when WiFi is connected. Show text in Serial and on the OLED (DO NOT REUSE AGAIN)

  //Start the Web Server
  defineHTMLRoutes(); //Fonction to call Routes to HTML Page bellow (DO NOT REUSE AGAIN)
  server.begin();  //Start the WEB Server (DO NOT REUSE AGAIN)
  delay(8000); //Wait 8 seconds

  //Call the different part of setup in independants functions
  chooseFunction(); //Call the chooseFunction function
  chooseIncrementation(); //Call the chooseIncrementation function
}

// ################# //
// ### MAIN LOOP ### //
// ################# //

void loop() {
  if(WiFi.status() != WL_CONNECTED) { //WiFi Disconnected
    displayPrintTextFull(true, true, 2, "WiFi      Disconnected"); //Tell the user to refresh the page (Keep the spaces to change lines)
    ledRED(); //Set the LED to RED
  }
  /*
  mappedIncrementationValue = map(analogRead(selectorPin), 0, 1023, (-incrementationRange), incrementationRange);
  if(digitalRead(rightButton)){ //The right button was pressed
    while(digitalRead(rightButton)); //Wait until button is released (Prevent action to run multiple time on same press)
    switch(selectedParameter){ //Look for the current selected parameter
      case 'a': //The current parameter is a
        selectedParameter = b; //Change the parameter a to the next one (b)
        break; //Stop the switch statement
      case 'b': //The current parameter is b
        selectedParameter = h; //Change the parameter b to the next one (h)
        break; //Stop the switch statement
      case 'h': //The current parameter is h
        selectedParameter = k; //Change the parameter h to the next one (k)
        break; //Stop the switch statement
      case 'k': //The current parameter is k
        selectedParameter = a; //Change the parameter k to the next one (a)
        break; //Stop the switch statement
    }
    Serial.println("Now changing " + selectedParameter + "parameter."); //Tell the user which parameter is currently been changed
  }
  if(digitalRead(leftButton)){ //The left button was pressed
    while(digitalRead(leftButton)); //Wait until button is released (Prevent action to run multiple time on same press)
    switch(selectedParameter){ //Look for the current selected parameter
      case 'a': //The current parameter is a
        a = a + mappedIncrementationValue; //Adds the mappedIncrementationValue to a
        break; //Stop the switch statement
      case 'b': //The current parameter is b
        b = b + mappedIncrementationValue; //Adds the mappedIncrementationValue to b
        break; //Stop the switch statement
      case 'h': //The current parameter is h
        h = h + mappedIncrementationValue; //Adds the mappedIncrementationValue to h
        break; //Stop the switch statement
      case 'k': //The current parameter is k
        k = k + mappedIncrementationValue; //Adds the mappedIncrementationValue to k
        break; //Stop the switch statement
    }
    Serial.println("Added " + mappedIncrementationValue + " to " + selectedParameter + " parameter."); //Tell the user the mappedIncrementationValue was added to the current parameter
  }
  */
}

// ####################### //
// ### SETUP FUNCTIONS ### //
// ####################### //

void chooseFunction(){
  ledBLUE(); //Set the LED to BLUE
  displayPrintTextFull(true, true, 2, "Check     Serial    Monitor"); //Print text on OLED (Keep the spaces to change lines)

  //Ask for the function mode
  Serial.println("Please input the desired function. Here are the available options : "); //Ask Question
  Serial.println("sin, cos, tan, abs, exp, quad, affine"); //List available options
  while(!Serial.available()); //Wait for user input
  functionMode = Serial.readString(); //Convert the Serial input to char in functionMode

  //Check which option was choosed by user (can't be a switch, because it only takes one char)
  if(functionMode == "sin"){ //the input was 'sin'
    Serial.println("You have selected the Sinus function! For this program we use the canonical form : f(x) = a*sin(b(x-h)) + k"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'b'? (ex.: -6.37)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b

    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h

    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k

    functionFinal = ("y = " + String(a) + "*sin(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else if(functionMode == "cos"){ //the input was 'cos'
    Serial.println("You have selected the Cosinus function! For this program we use the canonical form : f(x) = a*cos(b(x-h)) + k"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'b'? (ex.: -6.37)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b

    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h

    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k

    functionFinal = ("y = " + String(a) + "*cos(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else if(functionMode == "tan"){ //the input was 'tan'
    Serial.println("You have selected the Tangent function! For this program we use the canonical form : f(x) = a*tan(b(x-h)) + k"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'b'? (ex.: -6.37)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b

    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h

    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k

    functionFinal = ("y = " + String(a) + "*tan(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else if(functionMode == "abs"){ //the input was 'abs'
    Serial.println("You have selected the Absolute function! For this program we use the canonical form : f(x) = a*abs(x-h) + k"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h

    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k

    functionFinal = ("y = " + String(a) + "*abs(x-" + String(h) + ")+" + String(k)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else if(functionMode == "exp"){ //the input was 'exp'
    Serial.println("You have selected the Exponential function! For this program we use the canonical form : f(x) = a*exp(b(x-h)) + k"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'b'? (ex.: -6.37)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b

    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h

    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k

    functionFinal = ("y = " + String(a) + "*exp(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else if(functionMode == "quad"){ //the input was 'quad'
    Serial.println("You have selected the Quadratic function! For this program we use the canonical form : f(x) = a*((x-h)*exp(2)) + k"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'h'? (ex.: 7.90)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    h = Serial.parseFloat(); //Convert the Serial input to float in h

    Serial.println("What is 'k'? (ex.: 36.89)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    k = Serial.parseFloat(); //Convert the Serial input to float in k

    functionFinal = ("y = " + String(a) + "*((x-" + String(h) + ")*exp(2))+" + String(k)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else if(functionMode == "affine"){ //the input was 'affine'
    Serial.println("You have selected the Affine function! For this program we use the base form : f(x) = a*x + b"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'b'? (ex.: -6.37)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b

    functionFinal = ("y = " + String(a) + "*x+" + String(b)); //Ajoute la fonction à fonctionFinal
    Serial.println("Here is your selection : " + String(fonctionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, 1, functionFinal); //Show fonction on OLED

  } else { //the input was invalid, the user typed something not in the options
    Serial.println("\"" + String(functionMode) + "\" is an invalid mode! Program Stopped."); //Tell the user the error
    displayClear(); //Clear the display
    displayShow(); //Show the modification on the display
    while(true); //Make an infinite loop to 'stop' the program

  }
  Serial.println("You can now refresh the browser page to see the changes."); //Tell the user to refresh the page
  displayPrintTextFull(true, true, 2, "Please    Refresh   Browser"); //Tell the user to refresh the page (Keep the spaces to change lines)
  delay(5000); //Wait 5 seconds

  Serial.println("Is the function correct? Type \"NO\" if you want to try again or type anything else to continue the program."); //Ask the user if he want to try again
  while(!Serial.available()); //Wait for user input
  if(Serial.readString() == "NO"){ //If the user input is 'NO'
    return; //Make chooseFunction() again
  }
}

void chooseIncrementation(){
  Serial.println("You now have to choose the incrementation range for the potentiometer. The potentiometer will be use to adjust the variables.");
  Serial.println("You need to specify a value between 0 and 100. For example, if a = -25 and incrementation = 50 then the adjustement is from -25 (LOWEST) to 25 (HIGHEST).");
  Serial.println("What is the incrementation range? (no decimal)");
  while(!Serial.available()); //Wait for user input
  incrementationRange = Serial.parseInt(); //Convert the Serial input to int in incrementationRange
  if ((incrementationRange > 100)&&(incrementationRange < 0)){
    Serial.println(String(incrementationRange) + " is an incorrect value. Trying again.");
    return; //call chooseIncrementation() again
  }
  Serial.println("Your incrementation range is " + String(incrementationRange) + ". If this is incorrect, type \"NO\" to try again or type anything else to continue the program.");
  while(!Serial.available()); //Wait for user input
  if(Serial.readString() == "NO"){ //If the user input is 'NO'
    return; //call chooseIncrementation() again
  }
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

//Print the input text with choosed size
void displayPrintText(int size, String text){
  display.setTextSize(size); //1 or 2
  display.println(text);
}

//Print the input text with choosed size with clear and show
void displayPrintTextFull(bool clear, bool show, int size, String text){
  if(clear)displayClear();
  displayCursor(0, 0);
  displayPrintText(size, text);
  if(show)displayShow();
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

//DO NOT REUSE MORE THEN ONCE IN THE CODE
void wifiConnected(){
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  displayPrintTextFull(true, false, 2, "Connected to WiFi");
  display.println();
  displayPrintTextFull(false, true, 1, "IP: " + String(WiFi.localIP()));
}

//DO NOT REUSE MORE THEN ONCE IN THE CODE
void wifiConnecting(){
  Serial.println("Connecting to WiFi..."); 
  displayPrintTextFull(true, false, 2, "Connectingto WiFi..."); //(Keep the text like this to properly change lines on display)
  display.println();
  displayPrintTextFull(false, true, 1, "SSID: " + String(ssid));
}

// ######################################################################################################################################### //
// ######################################################################################################################################### //
// #################                                            !!! WARNING !!!                                            ################# //
// #################    THE REST OF THE CODE ARE FUNCTIONS TO SEND HTML TO BROWSER, HTML IS NOT ARDUINO LANGUAGE (C++)     ################# //
// #################      ALL THE HTML SYNTAX NEEDS TO BE INSIDE A MAIN " " AND ALL END OF LINES NEEDS A BACKSLASH \       ################# //
// #################   ADD A BACKSLASH \ IN FRONT OF SYNTAX CHARACTERS TO CANCEL THE EFFECT (EX.: \"HELLO\" --> "HELLO")   ################# //
// ######################################################################################################################################### //
// ######################################################################################################################################### //

//DO NOT REUSE MORE THEN ONCE IN THE CODE
void defineHTMLRoutes() {
  // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { //Receive the root page on the IP (Ex.: 192.168.137.69/)
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

  server.on("/data-viewer", HTTP_GET, [](AsyncWebServerRequest* request) { //Receive the data-viewer page on the IP (Ex.: 192.168.137.69/data-viewer)
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
            <p>Amplitude (a) = " + (String)a + "</p>\
            <p>Largeur (b) = " + (String)b + "</p>\
            <p>Offset X (h) = " + (String)h + "</p>\
            <p>Offset Y (k) = " + (String)k + "</p>\
          </div>\
        </header>\
      </div>\
    </body>\
    </html>");
  });

  server.on("/graph-plotter", HTTP_GET, [](AsyncWebServerRequest* request) { //Receive the graph-plotter page on the IP (Ex.: 192.168.137.69/graph-plotter)
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