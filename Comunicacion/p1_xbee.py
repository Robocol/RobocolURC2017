import threading
import serial

xb = serial.Serial("/dev/tty.usbserial-1410", baudrate=9600)

def StartServer():

	linea = ""

	while(True):
		linea += xb.read().decode()
		if "\n" in linea:
			print(linea)
			linea=""
	


threading.Thread(target=StartServer).start()

while(True):
	var = raw_input("")
	try:
		xb.write((var+"\n").encode())
	except:
		print("Error en escritura")