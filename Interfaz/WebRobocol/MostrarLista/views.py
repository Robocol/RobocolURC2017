from django.shortcuts import render,redirect,render_to_response
from django.http import HttpResponse
from django.http import Http404
import threading
import serial
import numpy as np
import socket
import time
import simplejson as json

HOST = '192.168.0.100'
PORT = 7070

DIRECCION_BLUETOOTH = "/dev/tty.usbserial-1420"
BAUD_RATE = 115200

SEPARADOR_POSITIVO = "#"
SEPARADOR_NEGATIVO = "!"

global ultimo_izquierdo
global ultimo_derecho

ultimo_izquierdo = 999
ultimo_derecho = 999

global ultimo_signo_izquierdo
global ultimo_signo_derecho

ultimo_signo_izquierdo = 999
ultimo_signo_derecho = 999


lock = threading.Lock()

global EnviarMensaje
EnviarMensaje = True

try:
	bluetoothSerial = serial.Serial(DIRECCION_BLUETOOTH, baudrate=BAUD_RATE)
	print("\t \t -- Conexion con XBee S8 inicializada --")
except:
	print("\033[1;31mNo se pudo inicializar la conexión con la XBee S8 \033[0;0m")


global rf_wifi
rf_wifi = False

s = socket.socket()
s.settimeout(1)
try:
	s.connect((HOST, PORT))
	s.send(str.encode("C+RF"))
except:
	print("\033[1;31mError en conexión Wi-Fi \033[0;0m")
s.close()

print("\t \t -- Modo de control RF --")


def enviarMensajeInicializacion():

	global EnviarMensaje

	while EnviarMensaje:
		print("Enviando Inicialización")
		lock.acquire()
		try:
			bluetoothSerial.write(("A1#").encode())
		except:
			print("\033[1;31mError en conexión Bluetooth \033[0;0m")


		s = socket.socket()
		s.settimeout(1)

		try:
			s.connect((HOST, PORT))
			s.send(str.encode("A1#"))
		except:
			print("\033[1;31mError en conexión Wi-Fi \033[0;0m")
		s.close()



		lock.release()

		time.sleep(1)

threading.Thread(target=enviarMensajeInicializacion).start()



def index(request):
	global rf_wifi
	return render(request, "index.html", {'estado':rf_wifi})

def cambiarEstado(request):

	global rf_wifi
	rf_wifi = request.POST.get('estado') == 'true'

	modo = "C+WIFI" if rf_wifi else "C+RF"

	print("Modo de control:",modo)

	s = socket.socket()
	s.settimeout(1)

	try:
		s.connect((HOST, PORT))
		s.send(str.encode(modo))
	except:
		print("\033[1;31mError en conexión Wi-Fi \033[0;0m")
	s.close()

	return HttpResponse("")

def ajax_izquierda(request):

	global EnviarMensaje
	cambiar = False

	global ultimo_izquierdo
	global ultimo_derecho

	global ultimo_signo_izquierdo
	global ultimo_signo_derecho
	
	x = request.POST.get("x", "")
	y = request.POST.get("y", "")

	xEntero = int(x)
	yEntero = int(y)

	if xEntero>0:
		PWM_IZQUIERDA = int((xEntero*80/130) - (yEntero*80/130))
		PWM_DERECHA = int(-yEntero*80/130)
	else:
		PWM_IZQUIERDA = int(-yEntero*80/130)
		PWM_DERECHA = int((-xEntero*80/130) - (yEntero*80/130))

	if(PWM_DERECHA > 80): PWM_DERECHA=80
	if(PWM_IZQUIERDA > 80): PWM_IZQUIERDA=80

	if(PWM_DERECHA < -80): PWM_DERECHA=-80
	if(PWM_IZQUIERDA < -80): PWM_IZQUIERDA=-80

	if (PWM_IZQUIERDA >= 0):
		StringIzquierda = "L"+str(PWM_IZQUIERDA)+"#"
	else:
		StringIzquierda = "L"+str(-PWM_IZQUIERDA)+"!"

	if (PWM_DERECHA >= 0):
		StringDerecha = "R"+str(PWM_DERECHA)+"#"
	else:
		StringDerecha = "R"+str(-PWM_DERECHA)+"!"

	if np.abs(ultimo_izquierdo-PWM_IZQUIERDA)>3 or np.abs(ultimo_derecho-PWM_DERECHA)>3:


		lock.acquire()


		global rf_wifi
		if rf_wifi:
			s = socket.socket()
			s.settimeout(1)
			try:
				s.connect((HOST, PORT))

				if ultimo_signo_izquierdo != np.sign(PWM_IZQUIERDA) and PWM_IZQUIERDA!=0 and ultimo_signo_izquierdo!=0:
					s.send(("L0#").encode())

				if ultimo_signo_derecho != np.sign(PWM_DERECHA) and PWM_DERECHA!=0 and ultimo_signo_derecho!=0:
					s.send(("R0#").encode())

				s.send((StringIzquierda+StringDerecha).encode())
			except:
				print("\033[1;31mError en conexión Wi-Fi \033[0;0m")


			s.close()



		else:
			try:
				if ultimo_signo_izquierdo != np.sign(PWM_IZQUIERDA) and PWM_IZQUIERDA!=0 and ultimo_signo_izquierdo!=0:
					bluetoothSerial.write(("L0#").encode())

				if ultimo_signo_derecho != np.sign(PWM_DERECHA) and PWM_DERECHA!=0 and ultimo_signo_derecho!=0:
					bluetoothSerial.write(("R0#").encode())

				bluetoothSerial.write((StringIzquierda+StringDerecha).encode())
			except:
				print("\033[1;31mError en conexión Bluetooth \033[0;0m")
				cambiar = True



		EnviarMensaje = cambiar
		print("Comando",StringIzquierda+StringDerecha)



		lock.release()


		ultimo_izquierdo = PWM_IZQUIERDA
		ultimo_derecho = PWM_DERECHA
		ultimo_signo_izquierdo = np.sign(PWM_IZQUIERDA)
		ultimo_signo_derecho = np.sign(PWM_DERECHA)

	return HttpResponse("")


def accionBrazo(request):

	idAccion = request.POST.get("id")

	mensajeEnviar = ""

	if idAccion == "btn_1_up":
		mensajeEnviar = "B100#"
	elif idAccion == "btn_2_up":
		mensajeEnviar = "V100#"
	elif idAccion == "btn_1_down":
		mensajeEnviar = "B100!"
	elif idAccion == "btn_2_down":
		mensajeEnviar = "V100!"
	elif idAccion == "btn_1_dot":
		mensajeEnviar = "B0#"
	elif idAccion == "btn_2_dot":
		mensajeEnviar = "V0#"
	elif idAccion == "btn_p_close":
		mensajeEnviar = "P100!"
	elif idAccion == "btn_p_open":
		mensajeEnviar = "P100#"
	elif idAccion == "btn_p_dot":
		mensajeEnviar = "P0#"

	lock.acquire()

	global rf_wifi
	if rf_wifi:
		s = socket.socket()
		s.settimeout(1)
		try:
			s.connect((HOST, PORT))
			s.send((mensajeEnviar).encode())
		except:
			print("\033[1;31mError en conexión Wi-Fi \033[0;0m")


		s.close()



	else:
		try:
			bluetoothSerial.write((mensajeEnviar).encode())
		except:
			print("\033[1;31mError en conexión Bluetooth \033[0;0m")

	print("Comando",mensajeEnviar)


	lock.release()

	return HttpResponse("")




def rpi(request):

	global ultimo_izquierdo
	global ultimo_derecho
	global rf_wifi
	
	control = "Wi-Fi" if rf_wifi else "RF"

	enviarIzq = ultimo_izquierdo if ultimo_izquierdo!=999 else 0
	enviarDer = ultimo_derecho if ultimo_derecho!=999 else 0


	return render(request, "rpi.html", {'RPM_izq':enviarIzq, 'RPM_der':enviarDer, 'control':control})

def actualizarRPM(request):

	global ultimo_izquierdo
	global ultimo_derecho

	respuesta = {}
	respuesta['i'] = ultimo_izquierdo if ultimo_izquierdo!=999 else 0
	respuesta['d'] = ultimo_derecho if ultimo_derecho!=999 else 0
	respuesta['c'] = "Wi-Fi" if rf_wifi else "RF"

	return HttpResponse(json.dumps(respuesta))





