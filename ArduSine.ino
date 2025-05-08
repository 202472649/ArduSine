//Include the library needed for this code
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
const char* ssid = "NOAHLAPTOP"; //WIFI NAME
const char* password = "61!3Vf05B3"; //WIFI PASSWORD

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL)
// On ESP8266:              D2(SDA), D1(SCL)
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Define the display

#define GREEN_LED 12 //Pin D6
#define BLUE_LED 14 //Pin D5
#define RED_LED 13 //Pin D7

#define selectorPin A0 //Pin A0 : Potentiometer
#define leftButton 0 //Pin D3
#define rightButton 2 //Pin D4

//Define the function mode here
String functionMode = "sin"; //Options : sin, cos, tan, abs, log, exp, aff

//Define the parameters for all functions
float a, b, h, k = 0;
char selectedParameter = 'a';

//Incrementation range for the potentiometer
int incrementationRange = 0;
int incrementationRangeNegative = 0;
int mappedIncrementationValue = 0;

//Define the variable to store the function
String functionFinal = "y=0";

AsyncWebServer server(80); //Configure web server on port 80 (HTTP)

// ################## //
// ### MAIN SETUP ### //
// ################## //

void setup() {
  Serial.begin(9600); //Initialize Serial Monitor with baud 9600
  delay(3000); //Initial delay

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
  Serial.println("Welcome! Program started.");

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
  server.begin(); //Start the WEB Server
  delay(8000); //Wait 8 seconds

  //Call the different part of setup in independants functions
  chooseFunction(); //Call the chooseFunction function
  chooseIncrementation(); //Call the chooseIncrementation function

  Serial.println("You can now use the potentiometer to adjust the values and left button to apply to the parameter or right button to change the selected parameter"); //Tell user he now can use buttons and potentiometer
}

// ################# //
// ### MAIN LOOP ### //
// ################# //

void loop() {
  mappedIncrementationValue = map(analogRead(selectorPin), 12, 1024, incrementationRangeNegative, incrementationRange); //Map readed value on potentiometer from negative incrementation to positive incrementation
  if(!digitalRead(rightButton)){ //The right button was pressed
    while(!digitalRead(rightButton)); //Wait until button is released (Prevent action to run multiple time on same press)
    switch(selectedParameter){ //Look for the current selected parameter
      case 'a': //The current parameter is a
        selectedParameter = 'b'; //Change the parameter a to the next one (b)
        if(functionMode == "abs")selectedParameter = 'h'; //Override the b value because abs doesn't have b
        break; //Stop the switch statement
      case 'b': //The current parameter is b
        selectedParameter = 'h'; //Change the parameter b to the next one (h)
        if(functionMode == "affine")selectedParameter = 'a'; //Override the h value because affine doesn't have h
        break; //Stop the switch statement
      case 'h': //The current parameter is h
        selectedParameter = 'k'; //Change the parameter h to the next one (k)
        break; //Stop the switch statement
      case 'k': //The current parameter is k
        selectedParameter = 'a'; //Change the parameter k to the next one (a)
        break; //Stop the switch statement
    }
    Serial.println("Now changing " + String(selectedParameter) + " parameter."); //Tell the user which parameter is currently been changed
  }
  if(!digitalRead(leftButton)){ //The left button was pressed
    while(!digitalRead(leftButton)); //Wait until button is released (Prevent action to run multiple time on same press)
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
    Serial.println("Added " + String(mappedIncrementationValue) + " to " + String(selectedParameter) + " parameter."); //Tell the user the mappedIncrementationValue was added to the current parameter
  }
  if(WiFi.status() != WL_CONNECTED) { //WiFi Disconnected
    displayPrintTextFull(true, true, true, 2, "WIFI NO   LONGER    AVAILABLE"); //Tell the user to refresh the page (Keep the spaces to change lines) (look at function bellow to understand displayPrintTextFull())
    ledRED(); //Set the LED to RED
  }
  server.begin(); //Prevents the WEB Server of shutting down in loop
  displayPrintTextFull(true, false, true, 1, "a = " + String(a)); //Show the parameters on OLED (look at function bellow to understand displayPrintTextFull())
  displayPrintTextFull(false, false, false, 1, "b = " + String(b)); //Show the parameters on OLED (look at function bellow to understand displayPrintTextFull())
  displayPrintTextFull(false, false, false, 1, "h = " + String(h)); //Show the parameters on OLED (look at function bellow to understand displayPrintTextFull())
  displayPrintTextFull(false, false, false, 1, "k = " + String(k)); //Show the parameters on OLED (look at function bellow to understand displayPrintTextFull())
  display.println(); //Skip line
  displayPrintTextFull(false, false, false, 1, "selected  = " + String(mappedIncrementationValue)); //Show stuff on OLED (look at function bellow to understand displayPrintTextFull())
  displayPrintTextFull(false, true, false, 1, "current  = " + String(selectedParameter)); //Show stuff on OLED (look at function bellow to understand displayPrintTextFull())
  delay(10); //Add delay to make sure it's not overloaded
  yield(); //Prevents crashing from watchdog when in loop (Needed on some models of non-arduino board)
}

// ####################### //
// ### SETUP FUNCTIONS ### //
// ####################### //

void chooseFunction(){
  ledBLUE(); //Set the LED to BLUE
  displayPrintTextFull(true, true, true, 2, "Check     Serial    Monitor"); //Print text on OLED (Keep the spaces to change lines) (look at function bellow to understand displayPrintTextFull())

  //Ask for the function mode
  Serial.println("Please input the desired function. Here are the available options : "); //Ask Question
  Serial.println("sin, cos, tan, abs, exp, affine"); //List available options
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

    functionFinal = ("y = " + String(a) + "*sin(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à functionFinal
    Serial.println("Here is your selection : " + String(functionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, true, 1, functionFinal); //Show fonction on OLED (look at function bellow to understand displayPrintTextFull())

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

    functionFinal = ("y = " + String(a) + "*cos(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à functionFinal
    Serial.println("Here is your selection : " + String(functionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, true, 1, functionFinal); //Show fonction on OLED (look at function bellow to understand displayPrintTextFull())

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

    functionFinal = ("y = " + String(a) + "*tan(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à functionFinal
    Serial.println("Here is your selection : " + String(functionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, true, 1, functionFinal); //Show fonction on OLED (look at function bellow to understand displayPrintTextFull())

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

    functionFinal = ("y = " + String(a) + "*abs(x-" + String(h) + ")+" + String(k)); //Ajoute la fonction à functionFinal
    Serial.println("Here is your selection : " + String(functionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, true, 1, functionFinal); //Show fonction on OLED (look at function bellow to understand displayPrintTextFull())

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

    functionFinal = ("y = " + String(a) + "*exp(" + String(b) + "(x-" + String(h) + "))+" + String(k)); //Ajoute la fonction à functionFinal
    Serial.println("Here is your selection : " + String(functionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, true, 1, functionFinal); //Show fonction on OLED (look at function bellow to understand displayPrintTextFull())

  } else if(functionMode == "affine"){ //the input was 'affine'
    Serial.println("You have selected the Affine function! For this program we use the base form : f(x) = a*x + b"); //Confirm user choice

    Serial.println("Lets start with the parameters. What is 'a'? (ex.: 12.56)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    a = Serial.parseFloat(); //Convert the Serial input to float in a

    Serial.println("What is 'b'? (ex.: -6.37)"); //Ask user for is choice
    while(!Serial.available()); //Wait for user input
    b = Serial.parseFloat(); //Convert the Serial input to float in b

    functionFinal = ("y = " + String(a) + "*x+" + String(b)); //Ajoute la fonction à functionFinal
    Serial.println("Here is your selection : " + String(functionFinal)); //Show fonction on Serial Monitor
    displayPrintTextFull(true, true, true, 1, functionFinal); //Show fonction on OLED (look at function bellow to understand displayPrintTextFull())

  } else { //the input was invalid, the user typed something not in the options
    Serial.println("\"" + String(functionMode) + "\" is an invalid mode! Program Stopped."); //Tell the user the error
    displayClear(); //Clear the display
    displayShow(); //Show the modification on the display
    while(true); //Make an infinite loop to 'stop' the program
  }
  Serial.println("You can now refresh the browser page to see the changes."); //Tell the user to refresh the page
  displayPrintTextFull(true, true, true, 2, "Please    Refresh   Browser"); //Tell the user to refresh the page (Keep the spaces to change lines) (look at function bellow to understand displayPrintTextFull())
  delay(5000); //Wait 5 seconds

  Serial.println("Is the function correct? Type \"N\" if you want to try again or type \"Y\" to continue with the program."); //Ask the user if he want to try again
  while(!Serial.available()); //Wait for user input
  if(Serial.read() == 'N'){ //If the user's input was 'N'
    chooseFunction(); //call chooseFunction() again
  }
}

void chooseIncrementation(){
  Serial.println("You now have to choose the incrementation range for the potentiometer. The potentiometer will be use to adjust the variables."); //Say stuff on serial monitor
  Serial.println("You need to specify a value between 0 and 100. For example, if a = -25 and incrementation = 50 then the adjustement will be from -25 (LOWEST) to 25 (HIGHEST)."); //Say stuff on serial monitor
  Serial.println("What is the incrementation range? (positive with no decimal)"); //Ask user for the incrementation range
  while(!Serial.available()); //Wait for user input
  incrementationRange = Serial.parseInt(); //Convert the Serial input to int in incrementationRange
  incrementationRangeNegative = -incrementationRange; //Define incrementationRangeNegative with the negative of incrementationRange
  if ((incrementationRange > 100)&&(incrementationRange < 0)){ //If the value is less than 0 or greater than 100
    Serial.println(String(incrementationRange) + " is an incorrect value. Trying again."); //Tell the user the input was incorrect
    return; //call chooseIncrementation() again
  }
  Serial.println("Your incrementation range is " + String(incrementationRange) + ". If this is incorrect, type \"N\" to try again or type \"Y\" to continue with the program."); //Confirm user choice
  while(!Serial.available()); //Wait for user input
  if(Serial.read() == 'N'){ //If the user's input was 'N'
    chooseIncrementation(); //call chooseIncrementation() again
  }
}

// ############################### //
// ### OTHERS FUNCTIONS BELLOW ### //
// ############################### //

//Clear the display
void displayClear(){
  display.clearDisplay(); //Clear the OLED display
}

//Show the modification on the display
void displayShow(){
  display.display(); //Show modification on OLED
}

//Set the Cursor placement on the display
void displayCursor(int x, int y){
  display.setCursor(x,y); //Set the cursor placement on OLED
}

//Print the input text with choosed size
void displayPrintText(int size, String text){
  display.setTextSize(size); //Set the text size on OLED (1 or 2)
  display.println(text); //Print text on OLED with line break at the end
}

//Print the input text with choosed size with clear and show
void displayPrintTextFull(bool clear, bool show, bool position, int size, String text){
  if(clear)displayClear(); //If clear is true = Clear the OLED display
  if(position)displayCursor(0, 0); //If position is true = Set the cursor placement on OLED
  displayPrintText(size, text); //Use the previous function
  if(show)displayShow(); //If show is true = Show modification on OLED
}

//Set RED led on and others off
void ledRED(){
  digitalWrite(GREEN_LED, LOW); //Green LED is off
  digitalWrite(BLUE_LED, LOW); //Blue LED is off
  digitalWrite(RED_LED, HIGH); //Red LED is on
}

//Set BLUE led on and others off
void ledBLUE(){
  digitalWrite(GREEN_LED, LOW); //Green LED is off
  digitalWrite(BLUE_LED, HIGH); //Blue LED is on
  digitalWrite(RED_LED, LOW); //Red LED is off
}

//Set GREEN led on and others off
void ledGREEN(){
  digitalWrite(GREEN_LED, HIGH); //Green LED is on
  digitalWrite(BLUE_LED, LOW); //Blue LED is off
  digitalWrite(RED_LED, LOW); //Red LED is off
}

//Function to show that the WIFI was connected and the IP on Serial monitor and OLED
// !! ONLY USE AT STARTUP ONCE !! //
void wifiConnected(){
  Serial.println("Connected to WiFi"); //Tell the user it's connected
  Serial.print("IP address: "); //Tell the IP label
  Serial.println(WiFi.localIP()); //Tell the IP value
  displayPrintTextFull(true, false, true, 2, "Connected to WiFi"); //Tell the user it's connected
  display.println(); //Skip a line
  display.setTextSize(1); //Set text size as 1
  display.print("IP: "); //Tell the IP label
  display.println(WiFi.localIP()); //Tell the IP value    **WIFI.localIP cannot be converted to String so needs to be in it's own thing**
  displayShow(); //Show the modification on OLED
}

//Function to show that the WIFI is connecting with the SSID on Serial monitor and OLED
// !! ONLY USE AT STARTUP ONCE !! //
void wifiConnecting(){
  Serial.println("Connecting to WiFi..."); //Tell the user it's connecting
  displayPrintTextFull(true, false, true, 2, "Connectingto WiFi..."); //Tell the user it's connecting     **(Keep the text like this to properly change lines on display)**
  display.println(); //Skip a line
  displayPrintTextFull(false, true, false, 1, "SSID: " + String(ssid)); //Tell the user the WIFI SSID it's searching for
}

// ######################################################################################################################################### //
// ######################################################################################################################################### //
// #################                                            !!! WARNING !!!                                            ################# //
// #################       THE REST OF THE CODE IS TO SEND HTML TO THE BROWSER, HTML IS NOT ARDUINO LANGUAGE (C++)         ################# //
// #################      ALL THE HTML SYNTAX NEEDS TO BE INSIDE A MAIN " " AND ALL END OF LINES NEEDS A BACKSLASH \       ################# //
// #################   ADD A BACKSLASH \ IN FRONT OF SYNTAX CHARACTERS TO CANCEL THE EFFECT (EX.: \"HELLO\" --> "HELLO")   ################# //
// #################                                            !!! WARNING !!!                                            ################# //
// ######################################################################################################################################### //
// ######################################################################################################################################### //

// Define a route to serve the HTML page
// !! ONLY USE AT STARTUP ONCE !! //
void defineHTMLRoutes() {
  //Receive the root page on the IP (Ex.: 192.168.137.69/)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    //Send the request in html to browser
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

  //Receive the data-viewer page on the IP (Ex.: 192.168.137.69/data-viewer)
  server.on("/data-viewer", HTTP_GET, [](AsyncWebServerRequest* request) {
    //Send the request in html to browser
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
          margin:20px 30px 10px 30px;\
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
            <p>" + (String)functionFinal + "</p>\
            <p>Function Mode = " + (String)functionMode + "</p>\
            <p>Amplitude (a) = " + (String)a + "</p>\
            <p>Largeur (b) = " + (String)b + "</p>\
            <p>Offset X (h) = " + (String)h + "</p>\
            <p>Offset Y (k) = " + (String)k + "</p>\
            <br>\
            <p>Incrementation Range = " + (String)incrementationRange + "</p>\
            <p>Currently Edited Parameter = " + (String)selectedParameter + "</p>\
            <p>Value selected on Potentiometer = " + (String)mappedIncrementationValue + "</p>\
          </div>\
        </header>\
      </div>\
    </body>\
    </html>");
  });

  //Receive the graph-plotter page on the IP (Ex.: 192.168.137.69/graph-plotter)
  server.on("/graph-plotter", HTTP_GET, [](AsyncWebServerRequest* request) {
    //Send the request in html to browser
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
          <h2>THIS PAGE DOES NOT REFRESH AUTOMATICALLY</h2>\
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