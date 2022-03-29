#include <MotionGenerator.h>
#include <util/atomic.h>

MotionGenerator *trapezoidalProfile = new MotionGenerator(3960, 7920, 0);

String inputString = "";
bool stringComplete = false;

// Entrada de la señal A del encoder (Cable amarillo).
const byte C1 = 3;
// Entrada de la señal B del encoder (Cable verde).
const byte C2 = 2;
//  Motor DC
const byte in1 = 6;
const byte in2 = 7;
const byte ena = 10;
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

bool timeout = false;
unsigned long lastTimeout = 0;
int target = 0;

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

  Serial.println("Target Pos Profile -u");
}

void loop()
{
  // If the trapezoidal profile is completed then we set a timeout.
  // Once the timeout is completed we reset control vars
  if (!timeout) {
    //  if (stringComplete) {
    //    Cv = (double)inputString.toInt();
    //    inputString = "";
    //    stringComplete = false;
    //  }

    //  int target = R;
    //  int target = R * sin(prevT / 1e6);

    //    float positionRef = 1000;
    target = trapezoidalProfile->update(10 * R);
    float trapezoidalVel = trapezoidalProfile->getVelocity();

    // PID constants
    float kp = 1;
    float kd = 0.080;
    float ki = 0.0;

    // time difference
    long currT = micros();
    float deltaT = ((float)(currT - prevT)) / (1.0e6);
    prevT = currT;

    int pos = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      pos = n;
    }
    // error
    int e = pos - target;

    // derivative
    float dedt = (e - eprev) / (deltaT);
    // integral
    eintegral = eintegral + e * deltaT;
    // control signal
    float u = kp * e + kd * dedt + ki * eintegral;

    // motor power
    float pwr = fabs(u);
    if (pwr > 255)
    {
      pwr = 255;
    }
    // motor direction
    int dir = 1;
    if (u < 0)
    {
      dir = -1;
    }
    // signal the motor
    setMotor(dir, pwr, ena, in1, in2);
    // store previous error
    eprev = e;

    if (trapezoidalProfile->getFinished()) {
      // Motors turn off
      setMotor(0, pwr, ena, in1, in2);
      timeout = true;
      lastTimeout = millis();
    }

    Serial.print(target);
    Serial.print(" ");
    Serial.print(pos);
    Serial.print(" ");
    Serial.print(trapezoidalVel);
    Serial.print(" ");
    Serial.print(-u);
    Serial.println();

  }

  if (timeout) {
    if (millis() - lastTimeout >= 2000) {
      timeout = false;

      //    Reset pulse count
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
        n = 0;
      }
      prevT = 0;
      eprev = 0;
      eintegral = 0;
      target = 0;
      trapezoidalProfile->init();
    }
    Serial.print(0);
    Serial.print(" ");
    Serial.print(0);
    Serial.print(" ");
    Serial.print(trapezoidalProfile->getVelocity());
    Serial.print(" ");
    Serial.print(0);
    Serial.println();
  }
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

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2)
{
  analogWrite(ena, pwmVal);
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
  ant = act;
  act = PIND & 12;

  if (ant == 0 && act == 4)
    n++;
  if (ant == 4 && act == 12)
    n++;
  if (ant == 8 && act == 0)
    n++;
  if (ant == 12 && act == 8)
    n++;

  if (ant == 0 && act == 8)
    n--;
  if (ant == 4 && act == 0)
    n--;
  if (ant == 8 && act == 12)
    n--;
  if (ant == 12 && act == 4)
    n--;
}
