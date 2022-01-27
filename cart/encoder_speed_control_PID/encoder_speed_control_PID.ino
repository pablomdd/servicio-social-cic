#include <util/atomic.h>

String inputString = "";
bool stringComplete = false;

const byte C1 = 3; // Entrada de la señal A del encoder (Cable amarillo).
const byte C2 = 2; // Entrada de la señal B del encoder (Cable verde).
//  Motor DC
const byte in1 = 6;
const byte in2 = 7;
const byte ena = 10;

volatile int n = 0;
volatile byte ant = 0;
volatile byte act = 0;

double N = 0.0; // Revoluciones por minuto calculadas.
int Cv = 0; // Variable de control (pwm)
unsigned long lastTime = 0; // Tiempo anterior
unsigned long sampleTime = 500; // Tiempo de muestreo
unsigned int R = 1980; // Número de muescas que tiene el disco del encoder.

volatile int posi = 0; // specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(C1, INPUT);
  pinMode(C2, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  digitalWrite(in1, false);
  digitalWrite(in2, false);
  analogWrite(ena, Cv);

  attachInterrupt(digitalPinToInterrupt(C1), encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2), encoder, CHANGE);

  Serial.println("Velocidad RPM");
}

void loop() {
//  if (stringComplete) {
//    Cv = (double)inputString.toInt();
//    inputString = "";
//    stringComplete = false;
//  }

//  int target = R;
 int target = R*sin(prevT/1e6);
 
 // PID constants
  float kp = 1;
  float kd = 0.080;
  float ki = 0.0;

  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  int pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = n;
  }  
  // error
  int e = pos - target;

  // derivative
  float dedt = (e-eprev)/(deltaT);

  // integral
  eintegral = eintegral + e*deltaT;

  // control signal
  float u = kp*e + kd*dedt + ki*eintegral;

    // motor power
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
  }

  // signal the motor
  setMotor(dir,pwr,ena,in1,in2);

  // store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(pos);
  Serial.println();
  
//  computeRpm();
}

//void serialEvent() {
//  while (Serial.available()) {
//    char inChar = (char)Serial.read();
//    inputString += inChar;
//    if (inChar == '\n') {
//      stringComplete = true;
//    }
//  }
//}

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){
  analogWrite(ena,pwmVal);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }  
}


void computeRpm(void) {
  if (millis() - lastTime >= sampleTime) { // Se actualiza cada segundo
    Serial.println(n);
    N = (n * 60.0 * 1000.0) / ((millis() - lastTime) * R); // Calculamos las revoluciones por minuto
    lastTime = millis(); // Almacenamos el tiempo actual.
    n = 0; // Inicializamos los pulsos.
    // Gira el motor en un sentido antihorario.
    if (Cv > 0) {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    } else {
      // Gira el motor en un sentido horario.
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
    analogWrite(ena, abs(Cv)); // Aplica señal de control (pwm).
    Serial.print("RPM: "); Serial.println(N);
  }
}


// Encoder precisión cuádruple.
void encoder(void) {
  ant = act;
  act = PIND & 12;

  if (ant == 0 && act == 4) n++;
  if (ant == 4 && act == 12) n++;
  if (ant == 8 && act == 0) n++;
  if (ant == 12 && act == 8) n++;

  if (ant == 0 && act == 8) n--;
  if (ant == 4 && act == 0) n--;
  if (ant == 8 && act == 12) n--;
  if (ant == 12 && act == 4) n--;

}
