import smbus
import time
import serial

xb = serial.Serial("/dev/ttyUSB0",baudrate=9600)

# for RPI version 1, use “bus = smbus.SMBus(0)”
bus = smbus.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

def writeNumber(number):
    xb.write("OK. Recibido: "+str(number)+"\n")
    bus.write_byte(address,number)
    # bus.write_byte_data(address, 0, value)
    return -1

def readNumber():
    line=""
    while "\n" not in line:
        line += xb.read()
    number = bus.read_byte(address)
    # number = bus.read_byte_data(address, 1)
    return line

while True:
    #var = input('Enter 1 – 9: ')
    #if not var:
    #    continue
    number = readNumber()
    number = str(number).strip("\n")
    writeNumber(int(number))
    print 'RPI: Hi Arduino, I sent you ', str(number)
    # sleep one second
    #time.sleep(1)
    print 'Arduino: Hey RPI, I received a digit ', str(number)
    print    
        
    
