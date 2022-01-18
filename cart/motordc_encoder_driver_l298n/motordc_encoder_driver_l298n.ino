#include <util/atomic.h>

// Motor 2
int ENB = 10;
int IN4 = 8;
int IN3 = 9;

// Motor 1
int ENA = 5;
int IN1 = 6;
int IN2 = 7;

// Encoder M1
int M1_ENCODER_A = 2;  // Amarillo
int M1_ENCODER_B = 3;  // Verde

//Variable Global MODO. velocidad = true
bool modo = true;

//Variable global de posición compartida con la interrupción
volatile int theta = 0;

//Variable global de pulsos compartida con la interrupción
volatile int pulsos = 0;
unsigned long timeold;
float resolution = 374.22;

//Variable Global Velocidad
int vel = 0;

//Variable Global Posicion
int ang = 0;

void setup (){
 // Declaramos todos los pines como salidas
 pinMode (ENA, OUTPUT);
 pinMode (ENB, OUTPUT);
 pinMode (IN1, OUTPUT);
 pinMode (IN2, OUTPUT);
 pinMode (IN3, OUTPUT);
 pinMode (IN4, OUTPUT);
 pinMode (M1_ENCODER_A, INPUT);
 pinMode (M1_ENCODER_B, INPUT);
 
 Serial.begin(9600);
 Serial.println("Serial begin");

   //Configurar Interrupción
  timeold = 0;
  attachInterrupt(digitalPinToInterrupt(M1_ENCODER_A), leerEncoder,RISING);
}
void Adelante (){
 //Dirección motor A
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 200); //Velocidad motor A
 //Dirección motor B
 digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 200); //Velocidad motor B
}

void AdelantePWM (int val){
 //Dirección motor A
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, val); //Velocidad motor A
 //Dirección motor B
 digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, val); //Velocidad motor B
}

void Atras (){
 //Dirección motor A
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, 128); //Velocidad motor A
 //Dirección motor B
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, 128); //Velocidad motor B
}
void Derecha (){
 //Dirección motor A
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 200); //Velocidad motor A
 //Dirección motor B
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, HIGH);
 analogWrite (ENB, 100); //Velocidad motor A
}
void Izquierda (){
 //Dirección motor A
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, 50); //Velocidad motor A
 //Dirección motor B
 digitalWrite (IN3, HIGH);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 150); //Velocidad motor A
}
void Parar (){
 //Dirección motor A
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 0); //Velocidad motor A
 //Dirección motor B
 digitalWrite (IN3, LOW);
 digitalWrite (IN4, LOW);
 analogWrite (ENB, 0); //Velocidad motor A

 
}
void loop (){
/*  
  Serial.println("Vuelta");
  for (int i = 0; i < 5000; i++) {
    Adelante ();
    int a = digitalRead(EncoderA);
    Serial.print("PhaseA ");
    Serial.println(a);
    int b = digitalRead(EncoderB);
    Serial.print("PhaseB ");
    Serial.println(b);
    delay(10);
  }
*/
 
  AdelantePWM(200);
  Serial.println("Adelante");
  delay (3000);
  AdelantePWM(150);
  delay(2000);
  Atras ();
  Serial.println("Atras ");
  delay (3000);
 /*
 Derecha ();
 delay (2000);
 Izquierda ();
 delay (2000);
 */
   Parar ();
   Serial.println("Parar");
   delay (4000);
}

void leerEncoder(){
  Serial.println("Encoder");
  //Lectura de Velocidad
  if(modo){
    pulsos++; //Incrementa una revolución
    Serial.println(pulsos);
  } else {
    //Lectura de Posición 
    int b = digitalRead(M1_ENCODER_B);
    if(b > 0){
      //Incremento variable global
      theta++;
    }
    else{
      //Decremento variable global
      theta--;
    }
  }
  obtenerVelocidad();
}

void obtenerVelocidad (){
  float posicion;
  float rpm;  
  Serial.println("obtener Velocidad");
  //Espera un segundo para el calculo de las RPM
  if (millis() - timeold >= 1000) {
    Serial.print("Calcular RPMs");
    //Modifica las variables de la interrupción forma atómica
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
      //rpm = float(pulsos * 60.0 / 374.22); //RPM
      rpm = float((60.0 * 1000.0 / resolution ) / (millis() - timeold) * pulsos);
      timeold = millis();
      pulsos = 0;
    }
    Serial.print("RPM: ");
    Serial.println(rpm);
    Serial.print("PWM: ");
    Serial.println(vel);
  }
}
