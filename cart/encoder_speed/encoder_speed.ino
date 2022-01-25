// Programa que lee el encoder de un motor DC y calcula la posición con los pasos contados
// Entrada de la señal A del encoder.
const int C1 = 3;
// Entrada de la señal B del encoder.
const int C2 = 2;
volatile int n = 0;
volatile byte ant = 0;
volatile byte act = 0;
unsigned long lastTime = 0;
unsigned long sampleTime = 100;
// Resolucion del encoder R = mH*s*r
unsigned int R = 1980;
double N = 0.0;

void setup()
{
  Serial.begin(9600);
  pinMode(C1, INPUT);
  pinMode(C2, INPUT);
  attachInterrupt(digitalPinToInterrupt(C1), encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(C2), encoder, CHANGE);

  Serial.println("Posicion en grados");
}

void loop()
{
  posValue();
}

void posValue(void)
{
  if (millis() - lastTime >= sampleTime)
  { // Se actualiza cada sampleTime (milisegundos)
    // Velocidad en RPM
    N = (n * 60.0 * 1000.0) / ((millis() - lastTime) * R); 
    // Almacenamos el tiempo actual.
    lastTime = millis(); 
    // Inicializamos los pulsos.
    n = 0; 
    Serial.print("Velocidad RPM: ");
    Serial.println(N);
  }
}

// Encoder precisión cuádruple.
void encoder(void)
{
  ant = act;
  // Leer puerto D, solo pines 3 y 2 (bin00001100=12).
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
