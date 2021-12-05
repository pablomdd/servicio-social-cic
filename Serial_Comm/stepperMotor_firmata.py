from pyfirmata import Arduino, OUTPUT, util
import time

PORT = "/dev/ttyACM0" 
tablaPasos = [ 
    "1000", 
    "1100", 
    "0100", 
    "0110", 
    "0010", 
    "0011", 
    "0001", 
    "1001"
]


if __name__ == '__main__':
    board = Arduino(PORT)
    print("Communication Successfully started")

    pinMotor1 = board.digital[8]     # 28BYJ48 - In1
    pinMotor2 = board.digital[9]     # 28BYJ48 - In2
    pinMotor3 = board.digital[10]     # 28BYJ48 - In3
    pinMotor4 = board.digital[11]     # 28BYJ48 - In4

    # Set up
    pinMotor1.mode = OUTPUT
    pinMotor2.mode = OUTPUT
    pinMotor3.mode = OUTPUT
    pinMotor4.mode = OUTPUT


    # Velocidad de Motor: max 800 - min 1000 o mas
    velocidadMotor = 1200 / 1000000 # microseg a segundos
    pasosPorVuelta = 4076
    cantidadPasos = 8
    contadorPasos = 0

    def escribirSalidas(paso: int):
        pinMotor1.write(int(tablaPasos[paso][0]))
        pinMotor2.write(int(tablaPasos[paso][1]))
        pinMotor3.write(int(tablaPasos[paso][2]))
        pinMotor4.write(int(tablaPasos[paso][3]))

    def sentidoHorario():
        print(contadorPasos)
        contadorPasos += 1
        if contadorPasos >= cantidadPasos:
            contadorPasos = 0
        escribirSalidas(contadorPasos)

    # lOOP
    while True:
        time.sleep(0.5)
        print("Vuelta")
        for i in range(0, pasosPorVuelta):
            escribirSalidas(cantidadPasos - 1 - i % cantidadPasos)
            time.sleep(velocidadMotor)
        # Apagar bobinas
        pinMotor1.write(0)
        pinMotor2.write(0)
        pinMotor3.write(0)
        pinMotor4.write(0)
        time.sleep(0.5)