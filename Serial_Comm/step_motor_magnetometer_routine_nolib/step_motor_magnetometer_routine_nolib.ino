// definicion de pines
const int pinMotor1 = 8;    // 28BYJ48 - In1
const int pinMotor2 = 9;    // 28BYJ48 - In2
const int pinMotor3 = 10;   // 28BYJ48 - In3
const int pinMotor4 = 11;   // 28BYJ48 - In4

//definicion variables
int velocidadMotor = 1200;  // variable para determinar la velocidad 
// 800 maxima - minima 1000 o mas
int contadorPasos = 0;      // contador para los pasos
int pasosPorVuelta = 4076;  // pasos para una vuelta completa

// Tablas con la secuencia de encendido 
// quitar signo de comentar a la necesaria)

//secuencia 1-fase usar velocidadMotor = 2000
//const int cantidadPasos = 4;
//const int tablaPasos[4] = { B1000, B0100, B0010, B0001 };

//secuencia 2-fases usar velocidadMotor = 2000
//const int cantidadPasos = 4;
//const int tablaPasos[4] = { B1001, B1100, B0110, B0011 };

//secuencia media fase usar velocidadMotor = 1000
const int cantidadPasos = 8;
const int tablaPasos[8] = { B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001 };


void setup(){
    //declarar pines como salida
    pinMode(pinMotor1, OUTPUT);
    pinMode(pinMotor2, OUTPUT);
    pinMode(pinMotor3, OUTPUT);
    pinMode(pinMotor4, OUTPUT);
}

void loop(){
    delay(2500);
    for (int i = 0; i < pasosPorVuelta + 100; i++) {
        sentidoHorario();
        delayMicroseconds(velocidadMotor);
    }
    // This delay helps the step motor to make the turn properly without getting stucked.
    delay(50);
    /*
    for (int i = 0; i < pasosPorVuelta; i++) {
        sentidoAntihorario();
        delayMicroseconds(velocidadMotor);
    }}

   */
    delay(1000);
}

void sentidoHorario() {
  contadorPasos++;
  if (contadorPasos >= cantidadPasos) contadorPasos = 0;
  escribirSalidas(contadorPasos);
}

void sentidoAntihorario(){
  contadorPasos--;
  if (contadorPasos < 0) contadorPasos = cantidadPasos - 1;
  escribirSalidas(contadorPasos);
}

void escribirSalidas(int paso)
{
  digitalWrite(pinMotor1, bitRead(tablaPasos[paso], 0));
  digitalWrite(pinMotor2, bitRead(tablaPasos[paso], 1));
  digitalWrite(pinMotor3, bitRead(tablaPasos[paso], 2));
  digitalWrite(pinMotor4, bitRead(tablaPasos[paso], 3));
}
