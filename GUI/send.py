from serial import *
log = open('log.txt','rt')
com = input('Enter COM Port : ')
ser = Serial(com,9600)
ch = log.readlines()
for i in range(len(ch)):
    print(ch)
    line=ch[i]
    if i==0:
        for j in range(len(line)):
            if j==0:
                ser.write(line[j].encode('utf-8'))
                print(line[j])
            else:
                while True:
                    mess=ser.read().decode('utf-8')
                    if (mess==line[j-1]):
                        break
                ser.write(line[j].encode('utf-8'))
                print(line[j])
        print('\n')        
        print('Line sent \n')

    else:
        while True:
                    mess=ser.read().decode('utf-8')
                    if (mess=='>'):
                        break
        print(mess)
        for j in range(len(line)):
            if j==0:
                ser.write(line[j].encode('utf-8'))
                print(line[j])
            else:
                mess=ser.read().decode('utf-8')
                while (mess!=line[j-1]):
                    pass
                ser.write(line[j].encode('utf-8'))
                print(line[j])
        print('\n')
        print('Line sent \n')
