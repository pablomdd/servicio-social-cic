from numpy.core.fromnumeric import take
from gi.repository import Gtk, GLib, Gio
from matplotlib.backends.backend_gtk3agg import (
    FigureCanvasGTK3Agg as FigureCanvas)
from matplotlib.figure import Figure
import numpy as np
import time
import threading
import serial
# from pyfirmata import Arduino, util
from stepper import StepperMotor
import glob
import sys
import gi
gi.require_version('Gtk', '3.0')

# TODO: Take constants into a separate file.
MAGNETOMETER = "Magnetometer"
ACCELEROMETER = "Accelerometer"
GYROSCOPE = "Gyroscope"

class AppWindow(Gtk.ApplicationWindow):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self.timer = None

        self.set_border_width(10)
        hpaned = Gtk.Paned.new(Gtk.Orientation.HORIZONTAL)
        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)

        # Label Serial Port
        serial_port_label = Gtk.Label.new("Serial Port:")
        vbox.pack_start(serial_port_label, False, True, 0)
        # Combobox Serial Port
        ports = self.getSerialPorts()
        port_combobox = Gtk.ComboBoxText()
        port_combobox.set_entry_text_column(0)
        port_combobox.connect("changed", self.on_port_change)
        for port in ports:
            port_combobox.append_text(str(port))
        port_combobox.set_active(0)
        self.port = str(port_combobox.get_active_text())
        vbox.pack_start(port_combobox, False, False, 0)

        # Label Samples
        samples_label = Gtk.Label.new("Samples: ")
        vbox.pack_start(samples_label, False, False, 0)

        # Spinbox samples
        samples_spin = Gtk.SpinButton.new_with_range(1, 1000, 10)
        samples_spin.set_digits(0)
        samples_spin.connect("value-changed", self.on_samples_changed)
        vbox.pack_start(samples_spin, False, False, 0)

        # Label Sensor Reading
        serial_port_label = Gtk.Label.new("MPU sensor to be read:")
        vbox.pack_start(serial_port_label, False, True, 0)
        # Combobox Serial Port
        sensor_options = [ACCELEROMETER, GYROSCOPE, MAGNETOMETER] # MPU options
        sensor_combobox = Gtk.ComboBoxText()
        sensor_combobox.set_entry_text_column(0)
        sensor_combobox.connect("changed", self.on_sensor_option_change)
        for option in sensor_options:
            sensor_combobox.append_text(str(option))
        sensor_combobox.set_active(2)
        vbox.pack_start(sensor_combobox, False, False, 0)
        # Button Start
        self.start_button = Gtk.Button.new_with_label("Start")
        self.start_button.connect("clicked", self.on_button_start)
        vbox.pack_start(self.start_button, False, False, 0)
        # Button Stop
        self.stop_button = Gtk.Button.new_with_label("Stop")
        self.stop_button.connect("clicked", self.on_button_stop)
        vbox.pack_start(self.stop_button, False, False, 0)
        # Button Save
        self.save_button = Gtk.Button.new_with_label("Save")
        self.save_button.connect("clicked", self.on_button_save)
        vbox.pack_start(self.save_button, False, False, 0)
        # Button Calibration
        self.stepper_motor_button = Gtk.Button.new_with_label("Stepper Routine")
        self.stepper_motor_button.connect("clicked", self.on_button_calibrate)
        vbox.pack_start(self.stepper_motor_button, False, False, 0)
        # Button Calibration
        self.calibrate_button = Gtk.Button.new_with_label("Calibrate")
        self.calibrate_button.connect("clicked", self.on_button_calibrate)
        vbox.pack_start(self.calibrate_button, False, False, 0)
        
        hpaned.add1(vbox)

        # App vars initialization
        self.current_sensor = str(sensor_combobox.get_active_text())     
        self.logic_level = 5.0
        # self.baud_rate = 9600
        self.baud_rate = 115200
        self.board_resolution = 1023
        self.samples = 0
        self.micro_board = None
        self.time_interval = 0.050  # seconds (s)
        self.values = []

        # Example sine wave plot on init
        self.fig = Figure(figsize=(5, 4), dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.x = np.arange(0.0, 3.0, 0.015)
        self.y = ((self.logic_level / 2) + (self.logic_level/2)) * \
            np.sin(2*np.pi*self.x)

        self.ax.plot(self.x, self.y, 'C1o--')
        self.ax.set_xlabel("Time (s)")
        self.ax.set_ylabel("Voltage (V)")
        self.ax.grid(visible=True)
        self.ax.set_title(f"Sample Graph")

        # Add Graph to Canvas
        self.canvas = FigureCanvas(self.fig)
        self.canvas.set_size_request(300, 250)
        hpaned.add2(self.canvas)

        self.add(hpaned)
        self.set_size_request(800, 600)
        self.show_all()

    def draw(self, x, y):
        self.ax.clear()
        self.ax.plot(x, y, 'C1o--')
        self.ax.set_xlabel("x")
        self.ax.set_ylabel("y")
        self.ax.grid(visible=True)
        self.ax.set_title(f"{self.current_sensor} reading.")
        self.canvas.draw()

    """ draw_magnetometer()
    Receives a numpy list X and Y to graph the elipsis read by the MPU Magnetometer 
    on the X and Y axis.
    """

    def draw_magnetometer(self, x, y):
        self.ax.clear()
        self.ax.plot(x, y, 'C1o--')
        self.ax.set_xlabel("x")
        self.ax.set_ylabel("y")
        self.ax.grid(visible=True)
        self.ax.set_title(f"Magnetometer reading.")
        
        for i in range(x.size):
            xitem = x[i]
            yitem = y[i]
            #etiqueta = "{:.1f}".format(xitem)
            etiqueta = str(i)
            self.ax.annotate(etiqueta, (xitem,yitem), textcoords="offset points",xytext=(0,10),ha="center")
    
        # self.ax.set_xlim([-50, 50])
        # self.ax.set_ylim([-50, 50])
        self.canvas.draw()

    """ draw_calibrated_magnetometer()
    Receives a numpy list X and Y to graph the elipsis read by the MPU Magnetometer 
    on the X and Y axis.
    """

    def draw_calibrated_magnetometer(self, x, y, mx ,my):
        self.ax.clear()
        self.ax.plot(x, y, 'C1o--')
        self.ax.plot(mx, my, 'C2o--')
        self.ax.set_xlabel("x")
        self.ax.set_ylabel("y")
        self.ax.grid(visible=True)
        self.ax.set_title("Magnetometer calibration")
        self.canvas.draw()

    """ getSerialPorts()
    Explore serial ports available and reuturn a list of string names.
    Works both on Windows and Linux.
    """

    def getSerialPorts(self) -> list:
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port, 9600)
                s.close()
                result.append(port)
            except:
                pass
        return result

    """ on_port_change()
    Updates the serial port when the combobox changes.
    """

    def on_port_change(self, combo):
        available_port = str(combo.get_active_text())
        if available_port != None:
            self.port = available_port
        else:
            self.port = None
            self.on_no_port_available(self)

    """ on_no_port_available()
    Shows an pop up window with an error message when no serial port is found.
    """

    def on_no_port_available(self, widget):
        port_dialog = Gtk.MessageDialog(transient_for=self,
                                        flags=0,
                                        message_type=Gtk.MessageType.ERROR,
                                        buttons=Gtk.ButtonsType.OK,
                                        text="No serial port available",
                                        title="Serial Port")
        port_dialog.run()
        port_dialog.destroy()

    """ on_samples_changed()
    Updates the amount of samples.
    """

    def on_samples_changed(self, samples_spin):
        self.samples = samples_spin.get_value_as_int()

    def on_sensor_option_change(self, combo):
        self.current_sensor = str(combo.get_active_text())

    """on_button_start()
    Start button starts a async thread that executes the get_time() 
    method to read from the arduino board.

    """

    def on_button_start(self, widget):
        print("Start")
        self.stepper_routine_thread = threading.Thread(target=self.stepper_routine)
        self.stepper_routine_thread.daemon = True
        self.timer = threading.Thread(target=self.get_time)
        self.event = threading.Event()
        self.timer.daemon = True

        self.stepper_routine_thread.start()
        self.timer.start()

    def stepper_routine(self):
        stepper = StepperMotor()
        stepper.routine()

    """get_time()
    This method reads the serial port from the arduino. It stores data in a Numpy 
    array t for time, and v for value read.
    """

    def get_time(self):
        time_value = value = count = 0
        self.x = np.array([])
        self.y = np.array([])
        self.start_button.hide()
        self.save_button.hide()
        self.stop_button.show()
        self.calibrate_button.hide()
        take_data = False

        if self.micro_board != None:
            print("Closing board before init")
            self.micro_board.close()
        # Initialiaze Serial Connection if there's a valid Serial port selected
        if self.port != None:
            try:
                print("Opening Serial Comm on port:", self.port)
                # Serial initialization
                self.micro_board = serial.Serial(
                    str(self.port), self.baud_rate, timeout=1)
                time.sleep(1)
                # Reset Buffer
                self.micro_board.reset_input_buffer()
                # Reading flag set to tue
                take_data = True
            except:
                if not self.event.is_set():
                    print("Stop")
                    # Stop thread
                    self.event.set()
                    self.timer = None
                GLib.idle_add(self.on_failed_connection)
                take_data = False
        else:
            print("No serial port available. Restart.")
        # Serial port reading when reading flag is true.
        if take_data:
            if time_value == 0:
                if self.current_sensor == MAGNETOMETER:
                    # stepper = StepperMotor()
                    # stepper.routine()
                    print("X (mT) \t Y (mT) \t Magnetometer")
                elif self.current_sensor == ACCELEROMETER:
                    print("X (mss) \t Y (mss) \t Accelerometer")
                elif self.current_sensor == GYROSCOPE:
                    print("X (rad) \t Y (rad) \t Gyroscope")
                else:
                    print("X () \t Y ()")
            while not self.event.is_set():
                # Stop when we get to the samples amount limit.
                if count >= self.samples:
                    print("Sampling completed - Stoping...")
                    # Stop thread
                    self.event.set()
                    # Reset timer
                    self.timer = None
                    # Close Serial connection
                    if self.micro_board != None:
                        self.micro_board.reset_input_buffer()
                        self.micro_board.close()
                    break
                    
                try:
                    # Read serial port and decode.
                    temp = str(self.micro_board.readline().decode('cp437'))
                    temp = temp.replace("\n", "")
                    mpu_reading = temp.split(",")
                    # Append reading into app graph vars
                    if self.current_sensor == MAGNETOMETER:
                        # XY Plane
                        print(mpu_reading[6], mpu_reading[7])
                        self.x = np.append(self.x, float(mpu_reading[6]))
                        self.y = np.append(self.y, float(mpu_reading[7]))
                    elif self.current_sensor == GYROSCOPE:
                        # XY Plane
                        print(mpu_reading[3], mpu_reading[4])
                        self.x = np.append(self.x, float(mpu_reading[4]))
                        self.y = np.append(self.y, float(mpu_reading[3]))
                    elif self.current_sensor == ACCELEROMETER:
                        # XY Plane
                        print(mpu_reading[0], mpu_reading[1])
                        self.x = np.append(self.x, float(mpu_reading[0]))
                        self.y = np.append(self.y, float(mpu_reading[1]))
                except Exception as e:
                    print("Cannot make reading. //", e)
                    pass
                # Reading delay
                time.sleep(self.time_interval)
                # Current sample count increase
                count += 1
                # Update time by our time interval
                time_value += self.time_interval

            time.sleep(0.5)
            # Draw reading after completed sampling.
            if self.current_sensor == MAGNETOMETER:
                self.draw_magnetometer(self.x, self.y)
            elif self.current_sensor == ACCELEROMETER:
                self.draw(self.x, self.y)
            elif self.current_sensor == GYROSCOPE:
                self.draw(self.x, self.y)
            # Show buttons adter sampling is completed
            self.start_button.show()
            self.save_button.show()
            self.stop_button.hide()
            if self.current_sensor == MAGNETOMETER:
                self.calibrate_button.show()
            else:
                self.calibrate_button.hide()

    """ on_faild_connection()
    Shows an pop up window with an error message when the initilization connection with the board failed.
    """

    def on_faild_connection(self):
        print("Failed Connection")
        failed_connection_dialog = Gtk.MessageDialog(transient_for=self,
                                                     flags=0,
                                                     message_type=Gtk.MessageType.ERROR,
                                                     text="Board communication error. No data will be taken",
                                                     title="Serial Error")
        failed_connection_dialog.run()
        failed_connection_dialog.destroy()

    def on_button_stop(self, widget):
        print("Stop Button")
        self.event.set()
        self.timer = None
        if self.micro_board != None:    
            self.micro_board.reset_input_buffer()
            self.micro_board.close()

    def on_button_save(self, widget):
        print("Save Button")
        self.save_button.hide()
        self.start_button.hide()
        save_dialog = Gtk.FileChooserDialog(
            title="Save file as...", parent=self, action=Gtk.FileChooserAction.SAVE)
        save_dialog.add_buttons(Gtk.STOCK_CANCEL,
                                Gtk.ResponseType.CANCEL,
                                Gtk.STOCK_SAVE,
                                Gtk.ResponseType.OK)
        filter_csv = Gtk.FileFilter()
        filter_csv.add_pattern("*.CSV")
        filter_csv.set_name("CSV")
        save_dialog.add_filter(filter_csv)
        response = save_dialog.run()
        # self.values.append(str(time_value) +
                    #                    "," + "{0:.4f}".format(value))
        if response == Gtk.ResponseType.OK:
            filename = save_dialog.get_filename()
            if not filename.endswith(".csv"):
                filename += ".csv"
            new_file = open(filename, 'w')
            new_file.write("Time(s),Voltage(V)" + "\n")
            for i in range(self.x.size):
                # Write Magnetometer reading from memory
                new_file.write("{0:.4f}".format(self.x[i]) + "," + "{0:.4f}".format(self.y[i]) + "\n")
                # new_file.write(self.values[i] + "\n")
            new_file.close()
        save_dialog.destroy()
        self.start_button.show()
        self.save_button.show()

    def on_button_calibrate(self, widget):
        print("Calibrate button")
        if not self.x[0] or not self.y[0]:
            print("Unable to make calibration. No data or data corrupted.")
            return
        
        mx,my = self.getMagnetometerCalibrationValues(self.x, self.y)
        self.draw_calibrated_magnetometer(self.x, self.y, mx, my)

    def getMagnetometerCalibrationValues(self, x, y):
        x_sf, y_sf, x_off, y_off = self.getMagnetometerCalibrationParameters(x, y)
        print(f"x_sf = {x_sf}, y_sf = {y_sf}, x_off = {x_off}, y_off = {y_off}")
        mx = np.array([])
        my = np.array([])
        for x_i, y_i in np.nditer([x, y]):
            mx_i = x_sf * x_i + x_off
            my_i = y_sf * y_i + y_off
            mx = np.append(mx, mx_i)
            my = np.append(my, my_i)
        return mx, my 

    def getMagnetometerCalibrationParameters(self, x, y):
        x_min = x.min()
        x_max = x.max()
        y_min = y.min()
        y_max = y.max()
        # Scale Factor
        x_sf = (y_max - y_min) / (x_max - x_min)
        y_sf = (x_max - x_min) / (y_max - y_min)
        if x_sf <= 1:
            x_sf = 1
        if y_sf <= 1:
            y_sf = 1
        # Offset
        x_off = ((x_max - x_min) / 2 - x_max) * x_sf
        y_off = ((y_max - y_min) / 2 - y_max) * y_sf
        return x_sf, y_sf, x_off, y_off

        

        
        

class Application(Gtk.Application):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self.window = None

    def do_activate(self):
        if not self.window:
            self.window = AppWindow(
                application=self, title="Single Point Measurement - PyGtk")
        self.window.show_all()
        self.window.save_button.hide()
        self.window.stop_button.hide()
        self.window.calibrate_button.hide()
        self.window.present()

    def do_shutdown(self):
        if self.window.micro_board != None:
            try:
                self.micro_board.close()
            except:
                pass
        print("Byeee")
        Gtk.Application.do_shutdown(self)
        if self.window:
            self.window.destroy()


if __name__ == "__main__":
    app = Application()
    app.run(sys.argv)
