fo = open("part.DXF",'rt')

fo1 = open("log.txt",'w')
lines=fo.read().splitlines()
for i in range(len(lines)):
    if lines[i].startswith('LINE'):
        for j in range(i,len(lines),1):
            if lines[j].startswith('AcDbLine'):
                fo1.write("L ")
                s=lines[j+2].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+4].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+8].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+10].split(".")
                fo1.write(s[0])
                fo1.write('\n')
                break;
                
                
            
    elif lines[i].startswith('CIRCLE'):        
        for j in range(i,len(lines),1):
            if lines[j].startswith('AcDbCircle'):
                fo1.write("C ")
                s=lines[j+2].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+4].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+8].split(".")
                fo1.write(s[0])
                fo1.write('\n')
                break;

    elif lines[i].startswith('ARC'):        
        for j in range(i,len(lines),1):
            if lines[j].startswith('AcDbCircle'):
                fo1.write("A ")
                s=lines[j+2].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+4].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+8].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                print(lines[j+2])
                print(lines[j+4])
                print(lines[j+8])
            if lines[j].startswith('AcDbArc'):
                s=lines[j+2].split(".")
                fo1.write(s[0])
                fo1.write(" ")
                s=lines[j+4].split(".")
                fo1.write(s[0])
                fo1.write('\n')
                print(lines[j+2])
                break;

fo.close()
fo1.close()
