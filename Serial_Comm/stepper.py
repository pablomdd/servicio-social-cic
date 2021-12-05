from pyfirmata import Arduino, OUTPUT
import time

class StepperMotor():
    """ Works with a 28BYJ48 stepper motor.
    Receives optionally a port:string for initialization. It's default set to '/dev/ttyACM0'
    """
    def __init__(self, port=None) -> None:
        self.PORT = port if port else "/dev/ttyACM0" 
        self.board = None
        # Coil activation lookup table
        self.step_size = 8
        self.step_table = [ 
            "1000", 
            "1100", 
            "0100", 
            "0110", 
            "0010", 
            "0011", 
            "0001", 
            "1001"
        ]
        # Motor speed: max 800 - min 100 0 or more (less is faster)
        self.motor_speed = 1200 / 1000000 # microseg to seconds
        # A 28BYJ48 stepper motor has 4076 steps
        self.steps_per_lap = 4076
        # Temporal step counter
        # self.steps = 0

    def routine(self):
        """routine()
        Initializes board, does a clockwise lap, waits 5 seconds and then returns to original position
        (counter clockwise lap).
        """
        print("Initializing stepper routine.")
        try:
            self.board = Arduino(self.PORT)
            print("Communication Successfully started on port", self.PORT)
        except Exception as e:
            print("Connection with the board was not successful", e)
            return

        # Pinout initialization
        pinMotor1 = self.board.digital[8]     # 28BYJ48 - In1
        pinMotor2 = self.board.digital[9]     # 28BYJ48 - In2
        pinMotor3 = self.board.digital[10]     # 28BYJ48 - In3
        pinMotor4 = self.board.digital[11]     # 28BYJ48 - In4
        # Set up
        pinMotor1.mode = OUTPUT
        pinMotor2.mode = OUTPUT
        pinMotor3.mode = OUTPUT
        pinMotor4.mode = OUTPUT

        def motor_write(paso: int):
            pinMotor1.write(int(self.step_table[paso][0]))
            pinMotor2.write(int(self.step_table[paso][1]))
            pinMotor3.write(int(self.step_table[paso][2]))
            pinMotor4.write(int(self.step_table[paso][3]))

        # Reading Lap
        time.sleep(0.25)
        print("Starting lap - Clockwise")
        for i in range(0, self.steps_per_lap):
            motor_write(i % self.step_size)
            time.sleep(self.motor_speed)
        # Turn off coils
        pinMotor1.write(0)
        pinMotor2.write(0)
        pinMotor3.write(0)
        pinMotor4.write(0)
        time.sleep(5)
        # Back to position lap
        print("Starting lap - Counter Clockwise")
        for i in range(0, self.steps_per_lap):
            motor_write(self.step_size - 1 - i % self.step_size)
            time.sleep(self.motor_speed)
        # Turn off coils
        pinMotor1.write(0)
        pinMotor2.write(0)
        pinMotor3.write(0)
        pinMotor4.write(0)

        self.board.exit()
        print("Routine completed.")

if __name__ == '__main__':
    stepper = StepperMotor()
    stepper.routine()
