from django.shortcuts import render,redirect,render_to_response
from django.http import HttpResponse
from django.http import Http404
import threading
import serial


DIRECCION_BLUETOOTH = "/dev/tty.usbserial-1410"
BAUD_RATE = 115200

SEPARADOR_POSITIVO = "#"
SEPARADOR_NEGATIVO = "!"

lock = threading.Lock()

bluetoothSerial = serial.Serial(DIRECCION_BLUETOOTH, baudrate=BAUD_RATE)

print("\t \t -- Conexion con XBee S8 inicializada --")

def index(request):
	return render(request, "index.html")

def ajax_izquierda(request):
	
	x = request.POST.get("x", "")
	y = request.POST.get("y", "")

	xEntero = int(x)
	yEntero = int(y)

	if xEntero>0:
		PWM_IZQUIERDA = int((xEntero*249/130) - (yEntero*249/130))
		PWM_DERECHA = int(-yEntero*249/130)
	else:
		PWM_IZQUIERDA = int(-yEntero*249/130)
		PWM_DERECHA = int((-xEntero*249/130) - (yEntero*249/130))

	if(PWM_DERECHA > 249): PWM_DERECHA=249
	if(PWM_IZQUIERDA > 249): PWM_IZQUIERDA=249

	if(PWM_DERECHA < -249): PWM_DERECHA=-249
	if(PWM_IZQUIERDA < -249): PWM_IZQUIERDA=-249

	if (PWM_IZQUIERDA >= 0):
		StringIzquierda = "L"+str(PWM_IZQUIERDA)+"#"
	else:
		StringIzquierda = "L"+str(-PWM_IZQUIERDA)+"!"

	if (PWM_DERECHA >= 0):
		StringDerecha = "R"+str(PWM_DERECHA)+"#"
	else:
		StringDerecha = "R"+str(-PWM_DERECHA)+"!"

	lock.acquire()
	bluetoothSerial.write((StringIzquierda+StringDerecha).encode())
	lock.release()

	return HttpResponse("")

