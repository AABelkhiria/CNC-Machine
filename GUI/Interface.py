from tkinter import *
import serial
import math

class Paint(object):
    DEFAULT_PEN_SIZE = 5.0
    DEFAULT_COLOR = 'black'
    SCREEN_W=300
    SCREEN_H=300
    
    def __init__(self):
    
        self.init_widgets()

        self.setup()
        self.master.mainloop()

        
    def init_widgets(self):
        self.master = Tk()
        
        self.line_button = Button(self.master, text='Line',command = lambda : self.set_tool('line'))
        self.line_button.grid(row=0,column=0)

        self.circle_button = Button(self.master, text='Circle',command = lambda : self.set_tool('circle'))
        self.circle_button.grid(row=0,column=1)

        self.arc_button = Button(self.master, text='Arc',command = lambda : self.set_tool(shape='arc'))
        self.arc_button.grid(row=0,column=2)

        self.point_button = Button(self.master, text='Point',command = lambda : self.set_tool(shape='point'))
        self.point_button.grid(row=0,column=3)

        self.draw_zone = Canvas(self.master,height=600,width=600,bg='white')
        self.draw_zone.grid(row=1,columnspan=5)

        self.com_input = Entry(self.master)
        self.com_input.insert(END,'Enter COM PORT')
        self.com_input.grid(row = 0, column = 4)

        self.connect_port_button = Button(self.master , text = 'Connect PORT',command = self.connect_port)
        self.connect_port_button.grid(row= 0, column = 5)

        
        self.test_button = Button(self.master, text='send',command = self.send)
        self.test_button.grid(row = 0, column = 6)

        
         #debug
        self.ux = Button(self.master, text='up x',command = self.ux)
        self.ux.grid(row=0,column= 7)
        self.dx = Button(self.master, text='down x',command = self.dx)
        self.dx.grid(row=0,column= 8)
        self.uy = Button(self.master, text='up y',command = self.uy)
        self.uy.grid(row=0,column= 9)
        self.dy = Button(self.master, text='down y',command = self.dy)
        self.dy.grid(row=0,column= 10)

        self.menubar = Menu(self.master)
        self.menu1 = Menu(self.menubar, tearoff=0)
        self.menu1.add_command(label="Nouveau", command=self.alert)
        self.menu1.add_command(label="Ouvrir", command=self.alert)
        self.menu1.add_separator()
        self.menu1.add_command(label="Quitter", command=self.master.destroy)
        self.menubar.add_cascade(label="Fichier", menu=self.menu1)
        self.menu2 = Menu(self.menubar, tearoff=0)
        self.menu2.add_command(label="Undo", command = lambda : self.undo(event=None))
        
        self.menu2.add_command(label="Redo", command=self.alert)
        self.menubar.add_cascade(label="Editer", menu=self.menu2)

        self.master.config(menu=self.menubar)
        self.master.title('UI')

        
#Setup method

    def setup(self):
        self.line_start_x = None
        self.line_start_y = None

        self.circle_start_x = None
        self.circle_start_y = None

        self.arc_center_x = None
        self.arc_center_y = None
        self.arc_x1 = None
        self.arc_y1 = None
        self.arc_first_click = 0
                
        self.tool_option = 'line'

        self.Line_objects = []
        self.Circle_objects = []
        self.Point_objects = []
        self.Arc_objects = []
        self.stack = []

        self.com_port = 'COM15'

        self.draw_zone.bind('<Button-1>', self.draw_start)
        self.draw_zone.bind('<B1-Motion>',self.draw_motion)
        self.draw_zone.bind('<ButtonRelease-1>',self.draw_end)
        self.master.bind('<Control-z>',self.undo)
        
        
#draw methods start/motion/end
    def draw_start(self,event):
        if self.tool_option=='line':
            self.line_start(event)
        elif self.tool_option == 'circle':
            self.circle_start(event)
        elif self.tool_option=='point':
            self.point_start(event)
        elif self.tool_option=='arc':
            if self.arc_first_click == 0:
                self.arc_start1(event)
            else:
                self.arc_start2(event)
                                         
    def draw_motion(self,event):
        if self.tool_option=='line':
            self.line_motion(event)
        elif self.tool_option == 'circle':
            self.circle_motion(event) 
        elif self.tool_option == 'arc':
            if self.arc_first_click == 0:
                self.arc_motion1(event)
            else:
                self.arc_motion2(event)
          
    def draw_end(self,event):
        if self.tool_option=='line':
            self.line_end(event)
        elif self.tool_option == 'circle':
            self.circle_end(event)
        elif self.tool_option == 'arc':
            if self.arc_first_click == 0:
                self.arc_end1(event)
            else:
                self.arc_end2(event)             


#draw_line Methods
        
    def line_start(self,event):
        self.line_start_x=event.x
        self.line_start_y=event.y
    def line_motion(self,event):
        self.draw_zone.delete('temp_line_objects')
        self.draw_zone.create_line(self.line_start_x,self.line_start_y,event.x,event.y,fill=self.DEFAULT_COLOR,smooth=1,tags='temp_line_objects')
    def line_end(self,event):
        self.draw_zone.delete('temp_line_objects')
        x=self.draw_zone.create_line(self.line_start_x,self.line_start_y,event.x,event.y,fill=self.DEFAULT_COLOR,smooth=1)
        self.Line_objects.append(x)
        self.stack.append(x)
        
#draw_circle Methods

    def circle_start(self,event):
        self.circle_start_x = event.x
        self.circle_start_y = event.y

    def circle_motion(self,event):
        self.draw_zone.delete('temp_circle_objects')              
        self.draw_zone.create_oval((self.circle_start_x),(self.circle_start_y),event.x,
                                   (abs(event.x-self.circle_start_x)+self.circle_start_y) if (event.y>self.circle_start_y) else  (self.circle_start_y-abs(event.x-self.circle_start_x))
                                                                                                                                  )
    def circle_end(self,event):
        self.draw_zone.delete('temp_circle_objects')

        #rayon = abs(x-start_x)/2
        x=self.draw_zone.create_oval((self.circle_start_x),(self.circle_start_y),event.x,
                                     (abs(event.x-self.circle_start_x)+self.circle_start_y) if (event.y>self.circle_start_y) else  (self.circle_start_y-abs(event.x-self.circle_start_x)))
        self.Circle_objects.append(x)
        self.stack.append(x)

        
#draw point Methods

    def point_start(self,event):
        x = self.draw_zone.create_line(event.x,event.y,event.x+1,event.y+1)
        self.Point_objects.append(x)
        self.stack.append(x)
        

#draw_arc methods

    def arc_start1(self,event) :
       self.arc_x1 = event.x
       self.arc_y1 = event.y
       self.draw_zone.create_line(event.x,event.y,event.x+1,event.y+1)
       print('click1')
    
    def arc_motion1(self,event):
       #self.draw_zone.delete('temp_arc_objects1')
       #self.draw_zone.create_arc(self.arc_x1,self.arc_y1,event.x,self.arc_y1+(self.arc_x1-event.x),tags='temp_arc_objects1')
       self.draw_zone.delete('arc0')
       #self.draw_zone.create_arc(self.arc_x1,self.arc_y1,event.x,event.y,extent=ex,tags='arc0')
       print('motion1')

    def arc_end1(self,event):
       self.arc_x2 = event.x
       self.arc_y2 = event.y
       self.draw_zone.create_line(event.x,event.y,event.x+1,event.y+1)
       self.arc_first_click=1
       print('end1')
              
    def arc_start2(self,event):
       print('second click')
       
    def arc_motion2(self,event):
       print('motion2')
       self.draw_zone.delete('arc0')
       a1 = (self.arc_y2 - self.arc_y1) / (self.arc_x2 - self.arc_x1)
       b2 = ( (self.arc_y1 + self.arc_y2) / 2) + ((self.arc_x1 + self.arc_x2 ) /(2 * a1) )
       b1 = event.y - (a1 * event.x) 
       self.arc_center_x = (b2 - b1) / (a1+(1/a1))
       self.arc_center_y = (a1 * self.arc_center_x) + b1
       self.draw_zone.create_line(self.arc_center_x,self.arc_center_y,self.arc_center_x+1,self.arc_center_y+1)
       ex = math.degrees(math.atan( math.sqrt(((self.arc_x1+self.arc_x2)/2 - self.arc_center_x)**2 + ((self.arc_y1+self.arc_y2)/2 - self.arc_center_y)**2) / math.sqrt(((self.arc_x1+self.arc_x2)/2 - self.arc_x1)**2 + ((self.arc_y1+self.arc_y2)/2 - self.arc_y1)**2) ))
       #start = math.degrees( math.atan( (self.arc_y1-self.arc_center_y) / (self.arc_x1-self.arc_center_x) )   )
       self.draw_zone.create_arc(self.arc_x1,self.arc_y1,self.arc_x2,self.arc_y2,extent=ex,tags='arc0')
                    
       
    def arc_end2(self,event):
       self.arc_first_click = 0
        
        
#tool setters

    def set_tool(self,shape):
         self.tool_option = shape

    def undo(self,event):
        if not self.stack:
            return
        x = self.stack.pop()
        self.draw_zone.delete(x)

    def test(self):
        for i in range(len(self.Line_objects)):
            x1,y1,x2,y2 = self.draw_zone.coords(self.Line_objects[i])
            print(x1,y1,x2,y2)

    def connect_port(self):
        self.com_port = self.com_input.get()
        self.ser = serial.Serial(self.com_port,9600)
        
    def alert(self):
        print('yo')

    def send(self):
        log = open('Log.txt','wt')
        for i in range(len(self.Line_objects)):
            while(not(self.ser.writable)):
                pass
            x1,y1,x2,y2 =self.draw_zone.coords(self.Line_objects[i])
            ch= 'L %d %d %d %d\n' % (x1,y1,x2,y2)
            log.write(ch)
        log.close()

    def ux(self):
        while (not(self.ser.writable())):
            pass
        a=self.ser.write('f'.encode('utf-8'))
        b=self.ser.write('\n'.encode('utf-8'))
        print(a)
        print(b)
    def dx(self):
        while (not(self.ser.writable())):
            pass
        a=self.ser.write('g'.encode('utf-8'))
        b=self.ser.write('\n'.encode('utf-8'))
        print(a)
        print(b)
    def uy(self):
        while (not(self.ser.writable())):
            pass
        a=self.ser.write('h'.encode('utf-8'))
        b=self.ser.write('\n'.encode('utf-8'))
        print(a)
        print(b)
    def dy(self):
        while (not(self.ser.writable())):
            pass
        a=self.ser.write('j'.encode('utf-8'))
        b=self.ser.write('\n'.encode('utf-8'))
        print(a)
        print(b)
        

if __name__ == '__main__':
    ge = Paint()
