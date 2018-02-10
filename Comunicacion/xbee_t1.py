import serial

xb = serial.Serial("/dev/ttyUSB0",baudrate=9600)

line=""
while(True):
    line+=xb.read()
    if("\n" in line):
        print(line)
        line=""
        xb.write("OK \n")
        
    

