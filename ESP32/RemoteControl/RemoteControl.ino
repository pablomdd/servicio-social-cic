#include <WiFi.h>
#include <WebSocketsServer.h>
//#include <MotionGenerator.h>
// #include <util/atomic.h>

// Constants
// Wifi access
const char* ssid = "INFINITUME957";
const char* password = "3672360107";

// Globals
WebSocketsServer webSocket = WebSocketsServer(80);
//MotionGenerator *trapezoidalProfile = new MotionGenerator(3960, 7920, 0);
String inputString = "";
bool stringComplete = false;

// TODO: change arduino UNO input declaration to suitable ESP32 pins 
// - [x]: Motor driver and pwm (in1, in2, ena)
// - [ ]: Encoder Interrupts
// - [ ]: Enocder function (port D reading)

// Entrada de la señal A del encoder (Cable amarillo).
const byte C1 = 34;
// Entrada de la señal B del encoder (Cable verde).
const byte C2 = 35;
//  Motor DC 1
const int in1 = 27;
const int in2 = 26;
const int ena = 14;
const int m1PwmChannel = 0;
//  Motor DC 2
const int in3 = 5;
const int in4 = 17;
const int enb = 16;
const int m2PwmChannel = 1;
// PWM config
const int freq = 30000;
const int resolution = 8;
int dutyCycle = 200;
// Pulsos
volatile int n = 0;
volatile byte ant = 0;
volatile byte act = 0;

// Revoluciones por minuto calculadas.
double N = 0.0;
// Variable de control (pwm)
int Cv = 0;
// Tiempo anterior
unsigned long lastTime = 0;
// Tiempo de muestreo
unsigned long sampleTime = 500;
// Número de muescas que tiene el disco del encoder.
unsigned int R = 1980;

// specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
volatile int posi = 0;
long prevT = 0;
float eprev = 0;
float eintegral = 0;
int target = 0;
// If the trapezoidal profile is completed then we set a timeout.
// Once the timeout is completed we reset control vars
bool timeout = false;
unsigned long lastTimeout = 0;

bool newAction = false;
String action = "";

void setAction(uint8_t * payload) {
  String payloadStr = (char*)payload;
/*  
  switch (payloadStr)
  {
  // Forwards
  case "FW":
    setMotor(1, 255, m1PwmChannel, in1, in2);
    setMotor(1, 255, m2PwmChannel, in3, in4);
    break;
  // Backwards
  case "BC":
    setMotor(-1, 255, m1PwmChannel, in1, in2);
    setMotor(-1, 255, m2PwmChannel, in3, in4);
    break;
  // LEFT
  case "LF":
    setMotor(1, 200, m1PwmChannel, in1, in2);
    setMotor(1, 100, m2PwmChannel, in3, in4);
    break;
  // RIGHT
  case "RG":
    setMotor(1, 100, m1PwmChannel, in1, in2);
    setMotor(1, 200, m2PwmChannel, in3, in4);
    break;
  // STOP
  case "ST": 
  default:
    setMotor(0, 0, m1PwmChannel, in1, in2);
    setMotor(0, 0, m2PwmChannel, in3, in4);
    break;
  }
*/
  if (payloadStr == "FW"){
    setMotor(1, 255, m1PwmChannel, in1, in2);
    setMotor(1, 255, m2PwmChannel, in3, in4);
  } else if (payloadStr == "BC"){
    setMotor(-1, 255, m1PwmChannel, in1, in2);
    setMotor(-1, 255, m2PwmChannel, in3, in4);
  } else if (payloadStr == "LF"){
    setMotor(1, 200, m1PwmChannel, in1, in2);
    setMotor(1, 150, m2PwmChannel, in3, in4);
  } else if (payloadStr == "RG"){
    setMotor(1, 150, m1PwmChannel, in1, in2);
    setMotor(1, 200, m2PwmChannel, in3, in4);
  } else {
    setMotor(0, 0, m1PwmChannel, in1, in2);
    setMotor(0, 0, m2PwmChannel, in3, in4);
  }
}

void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch(type) {
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Echo text message back to client
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      setAction(payload);
      webSocket.sendTXT(num, payload);
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void setup() {

  // Start Serial port
  Serial.begin(115200);

  // Connect to access point
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  // Print our IP address
  Serial.println("\nConnected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());
  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  // Motor 1 config
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  digitalWrite(in1, false);
  digitalWrite(in2, false);
  analogWrite(ena, Cv);
  // configure LED PWM utility for writing PWM
  ledcSetup(m1PwmChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ena, m1PwmChannel);
  // Motor 2 config
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);
  digitalWrite(in3, false);
  digitalWrite(in4, false);
  analogWrite(enb, Cv);
  ledcSetup(m1PwmChannel, freq, resolution);
  ledcAttachPin(enb, m2PwmChannel);

  /*  
  // Motor Encoder
  pinMode(C1, INPUT);
  pinMode(C2, INPUT);
  // Encoder interrupt
  // attachInterrupt(digitalPinToInterrupt(C1), encoder, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(C2), encoder, CHANGE);

  Serial.println("Target Pos Profile -u");
  */
}

void loop() {
  // Look for and handle WebSocket data
  webSocket.loop();

  //  computeRpm();
}


//Demo routine for motors
// void loop() {
//   // Move the DC motor forward at maximum speed
//   Serial.println("Moving Forward");
//   // digitalWrite(motor1Pin1, LOW);
//   // digitalWrite(motor1Pin2, HIGH); 
//   setMotor(1, 255, m1PwmChannel, in1, in2);
//   setMotor(1, 255, m2PwmChannel, in3, in4);
//   delay(2000);

//   // Stop the DC motor
//   Serial.println("Motor stopped");
//   setMotor(0, 0, m1PwmChannel, in1, in2);
//   setMotor(0, 0, m2PwmChannel, in3, in4);
//   delay(1000);

//   // Move DC motor backwards at maximum speed
//   Serial.println("Moving Backwards");
//   setMotor(-1, 255, m1PwmChannel, in1, in2);
//   setMotor(-1, 255, m2PwmChannel, in3, in4);
//   delay(2000);

//   // Stop the DC motor
//   Serial.println("Motor stopped");
//   setMotor(0, 0, m1PwmChannel, in1, in2);
//   setMotor(0, 0, m2PwmChannel, in3, in4);
//   delay(1000);

//   // Move DC motor forward with increasing speed
//   while (dutyCycle <= 255){
//     // ledcWrite(m1PwmChannel, dutyCycle);  
//     setMotor(1, dutyCycle, m1PwmChannel, in1, in2);
//     setMotor(1, dutyCycle, m2PwmChannel, in3, in4);
//     Serial.print("Forward with duty cycle: ");
//     Serial.println(dutyCycle);
//     dutyCycle = dutyCycle + 5;
//     delay(500);
//   }
//   dutyCycle = 200;
// }


void setMotor(int dir, int pwmVal, int pwmChannel, int in1, int in2)
{
  
  // analogWrite(pwm, pwmVal);
  // PWM template: ledcWrite(pwmChannel, dutyCycle);
  ledcWrite(pwmChannel, pwmVal);
  if (dir == 1)
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else if (dir == -1)
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}


void computeRpm(void)
{
  // Se actualiza cada segundo
  if (millis() - lastTime >= sampleTime)
  {
    Serial.println(n);
    // Calculamos las revoluciones por minuto
    N = (n * 60.0 * 1000.0) / ((millis() - lastTime) * R);
    // Almacenamos el tiempo actual.
    lastTime = millis();
    // Inicializamos los pulsos.
    n = 0;
    // Gira el motor en un sentido antihorario.

    if (Cv > 0)
    {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
    else
    {
      // Gira el motor en un sentido horario.
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
    // Aplica señal de control (pwm).
    analogWrite(ena, abs(Cv));
    Serial.print("RPM: ");
    Serial.println(N);
  }
}


// Encoder precisión cuádruple.
void encoder(void)
{
  // TODO: Change reading from the D port. 
  /*
   Maybe just reading from the declared encoder inputs.
   
   Remember the PIND & 12 operation reads the entire port D and 
   only takes the reading from the encoder ports.
   eg: portD: MSB-> xxxxbbxx & 12=00001100 
    so posible values for the port are 0000=0, 0100=4, 1000=8, 11000=12
    thus, be bellow numeric comparisons
   */
//  ant = act;
//  const byte enc1 = digitalRead(C1); 
//  const byte enc2 = digitalRead(C1);
//  act = (((enc1 << 1) | enc2) << 2) & 12;
//
//  if (ant == 0 && act == 4)
//    n++;
//  if (ant == 4 && act == 12)
//    n++;
//  if (ant == 8 && act == 0)
//    n++;
//  if (ant == 12 && act == 8)
//    n++;
//
//  if (ant == 0 && act == 8)
//    n--;
//  if (ant == 4 && act == 0)
//    n--;
//  if (ant == 8 && act == 12)
//    n--;
//  if (ant == 12 && act == 4)
//    n--;
  n++;
  Serial.println(n);
}
