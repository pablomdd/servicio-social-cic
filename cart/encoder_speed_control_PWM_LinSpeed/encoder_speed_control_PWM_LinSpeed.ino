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

double u = 0.0; // Revoluciones por minuto calculadas.
int Cv = 0; // Variable de control (pwm)
unsigned long lastTime = 0; // Tiempo anterior
unsigned long sampleTime = 500; // Tiempo de muestreo
unsigned int R = 1980; // Número de muescas que tiene el disco del encoder.

// Convert rpm -> rad/s. Computed constant value w = 2*pi*1000.0 / R
// Linear Speed u = d_wheel/2 * u = 0.067[m] * w = [m/s]
const float constValue = 0.1063;

void setup() {
  Serial.begin(9600);
  pinMode(C1, INPUT);
  pinMode(C2, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, false);
  digitalWrite(in2, false);
  analogWrite(ena, Cv);

  attachInterrupt(digitalPinToInterrupt(C1), encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2), encoder, CHANGE);

  Serial.println("Velocidad Lineal m/s");
}

void loop() {
  if (stringComplete) {
    Cv = (double)inputString.toInt();
    inputString = "";
    stringComplete = false;
  }

  computeRpm();
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


void computeRpm(void) {
  
  if (millis() - lastTime >= sampleTime) { // Se actualiza cada segundo
    // Velocidad Lineal 
    u =(constValue * n)/(millis()-lastTime);
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
    Serial.print("m/s: "); Serial.println(u);
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
