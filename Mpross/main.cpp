#include "mbed.h"
#include "string"
#include "stdio.h"

#define M_PI   3.141592653589793238462643383279502884L /* pi */
#define m      0.0018

using namespace std;
const int scale=200*0.5;  // scale 2 digits after the decimal point --  1mm matches 3.33 stepper rounds

DigitalOut  myled(LED1);
BusOut      mot1(D3,D2,D4,D5);
BusOut      mot2(D8,D9,D7,D6);

BusOut mot3(D13,D11,D12,D10);

DigitalIn capx(A0);   // mot1 == x
DigitalIn capy(A1);
DigitalIn capz(A2);  // mot2 == y
Serial ser(USBTX, USBRX,9600);
unsigned int X;
unsigned int Y;
unsigned int Z;
string buffer;

struct pos 
{
  int val;
  int indice;
};

pos state1={0,1};
pos state2={0,1};
pos state3={0,1};

pos tab[4]={ {0b0011,0},{0b0110,1},{0b1100,2},{0b1001,3} };

int arrondi(float nombre)
{
  return nombre + 0.5;
}

void release()
{
    mot1=0;
    mot2=0;
    mot3=0;
}

int sqr(int x)
{
return x*x;
}

void stepx(int dir) 
{
  if (dir==1)         //clockwise
  {
    if (!(state1.indice==3))
    {
      state1=tab[state1.indice+1];
      mot1=state1.val;
    }
    else
    {
      state1=tab[0];
      mot1=state1.val;
    }
  }
  else                //counterclockwise
  {
    if (!(state1.indice==0))
    {
      state1=tab[state1.indice-1];
      mot1=state1.val;
    }
    else
    {
      state1=tab[3];
      mot1=state1.val;
    }
  }
}

void stepy(int dir)
{
  if (dir==1)         //clockwise
  {
    if (!(state2.indice==3))
    {
      state2=tab[state2.indice+1];
      mot2=state2.val;
    }
    else
    {
      state2=tab[0];
      mot2=state2.val;
    }
  }
  else                //counterclockwise
  {
    if (!(state2.indice==0))
    {
      state2=tab[state2.indice-1];
      mot2=state2.val;
    }
    else
    {
      state2=tab[3];
      mot2=state2.val;
    }
  }
}

void stepz(int dir)
{
  if (dir==1)           //clockwise
  {
    if (!(state3.indice==3))
    {
      state3=tab[state3.indice+1];
      mot3=state3.val;
    }
    else
    {
      state3=tab[0];
      mot3=state3.val;
    }
  }
  else                  //counterclockwise
  {
    if (!(state3.indice==0))
    {
      state3=tab[state3.indice-1];
      mot3=state3.val;
    }
    else
    {
      state3=tab[3];
      mot3=state3.val;
    }
  }
}

 void Line(int x1,int y1,int x2,int y2)
 {
  // go to (x1,y1)
  long int dx=x2-x1;
  long int dy=y2-y1;
  int sens_x,sens_y;

  if (x2>x1)
    sens_x=1;
  else
    sens_x=0;

  if (y2>y1)
    sens_y=1;
  else
    sens_y=0;

  dx=scale*abs(dx);
  dy=scale*abs(dy);

  if ((dx==0) && (dy==0))
  {
     // do nothing
  }
  else if (dx==0)
  {
    for (long int y=0; y < dy; y++) {
      stepy(sens_y); 
      wait(m);
    } 
  }
  else if (dy==0)
  {
    for (long int x=0; x < dx ; x++)
    {
      stepx(sens_x);
      wait(m);
    }
  }
  else if (dx==dy)
  {
    for (long int x=0; x < dx ;x++)
    {
      stepx(sens_x);
      stepy(sens_y);
      wait(m);
    }
  } 
  else if (dx>dy)
  {
    for (long int x=0; x < dx;x++) {
      float b=dx/dy;
      long int pente = arrondi(b);
      if (!(  (x+1) % pente  ))
      {
        stepx(sens_x);
        stepy(sens_y);
        wait(m);
        release();
      }
      else
      {
        stepx(sens_x);
        wait(m);
        //release();
      }
    }
  }
  else
  {
    for (long int y=0; y < dy;y++)
    {
      float a=dy/dx;
      long int pente= arrondi(a);
      if  (!(  (y+1) % pente  ))
      {
        stepx(sens_x);
        stepy(sens_y);
        wait(m);
        release();
      }
      else
      {
        stepy(sens_y);
        wait(m);
        //release();
      }
    }
  }
  release();
}

void Circle(int centerx,int centery,int r)
{
  // go (xcenter+r,centery)
  int scaledr=r*scale;
  int x=scaledr;          // a multiplier par 200 * scale
  int y=0;
     
  // (r,0)
  while(y<x)
  {
    int e1=abs(sqr(x)+sqr(y+1)-sqr(scaledr));
    int e2=abs(sqr(x-1)+sqr(y+1)-sqr(scaledr));
    if (e1<=e2)
    {
      y++;
      stepy(1);
      wait(m);
    }
    else
    {
      y++;
      x--;
      stepy(1);
      stepx(-1);
      wait(m);
      release();
    }
    release();
  }
  
  while (x!=0){
    int e1=abs(sqr(x-1)+sqr(y)-sqr(scaledr));
    int e2=abs(sqr(x-1)+sqr(y+1)-sqr(scaledr));
    if (e1<=e2)
    {
      x--;
      stepx(-1);
      wait(m);
    }
    else
    {
      y++;
      x--;
      stepy(1);
      stepx(-1);
      wait(m);
      release();
    }
    release();
  }
  
  while(y>-x)
  {
    int e1=abs(sqr(x-1)+sqr(y)-sqr(scaledr));
    int e2=abs(sqr(x-1)+sqr(y-1)-sqr(scaledr));
    if (e1<=e2)
    {
      x--;
      stepx(-1);
      wait(m);
    }
    else
    {
      y--;
      x--;
      stepy(-1);
      stepx(-1);
      wait(m);
      release();
    }
    release();
  }
  
  //
  while (y!=0){
        int e1=abs(sqr(x)+sqr(y-1)-sqr(scaledr));
        int e2=abs(sqr(x-1)+sqr(y-1)-sqr(scaledr));
        if (e1<=e2) {
            y--;
            stepy(-1);
            wait(m);
        }
        else {
            y--;
            x--;
            stepy(-1);
            stepx(-1);
            wait(m);
            release();
        }
        release();

  }
  //
  while(x<y){
        int e1=abs(sqr(x)+sqr(y-1)-sqr(scaledr));
        int e2=abs(sqr(x+1)+sqr(y-1)-sqr(scaledr));
        if (e1<=e2) {
            y--;
            stepy(-1);
            wait(m);
        }
        else {
            y--;
            x++;
            stepy(-1);
            stepx(1);
            wait(m);
            release();
        }
        release();

  }
  //
  while (x!=0){
        int e1=abs(sqr(x+1)+sqr(y)-sqr(scaledr));
        int e2=abs(sqr(x+1)+sqr(y-1)-sqr(scaledr));
        if (e1<=e2) {
            x++;
            stepx(1);
            wait(m);
        }
        else {
            y--;
            x++;
            stepy(-1);
            stepx(1);
            wait(m);
            release();
        }
        release();

  }
  //
  while(x<-y){
        int e1=abs(sqr(x+1)+sqr(y)-sqr(scaledr));
        int e2=abs(sqr(x+1)+sqr(y+1)-sqr(scaledr));
        if (e1<=e2) {
            x++;
            stepx(1);
            wait(m);
            
        }
        else {
            y++;
            x++;
            stepy(1);
            stepx(1);
            wait(m);
            release();
        }
        release();

  }
  //
  while (y!=0){
        int e1=abs(sqr(x)+sqr(y+1)-sqr(scaledr));
        int e2=abs(sqr(x+1)+sqr(y+1)-sqr(scaledr));
        if (e1<=e2) {
            y++;
            stepy(1);
            wait(m);
        }
        else {
            y++;
            x++;
            stepy(1);
            stepx(1);
            wait(m);
            release();
        }
    release();

  }




}




void Arc(int centerx,int centery,int x1,int y1,int x2,int y2){
// goto (x1,y1)
int octant;
int x=x1-centerx;
int y=y1-centery;
int r2=sqr(x)+sqr(y);
int xf=x2-centerx;
int yf=y2-centery;
// determination de l'octant de depart de l'arc
if (x1>centerx) {
    if (y1>centery)
    {
        if (sqr(x1-centerx)*2 > r2)
            octant = 1;
        else
            octant = 2;
    }
    else
    {
        if (sqr(x1-centerx)*2 > r2)
            octant = 8;
        else
            octant = 7;    
    }
}
else
{
    if (y1>centery)
    {
        if (sqr(x1-centerx)*2 > r2)
            octant = 4;
        else
            octant = 3;
    }
    else
    {
        if (sqr(x1-centerx)*2 > r2)
            octant = 5;
        else
            octant = 6;
    }
}

//
switch (octant) {
case 1 :  goto oct1;
case 2 :  goto oct2;
case 3 :  goto oct3;
case 4 :  goto oct4;
case 5 :  goto oct5;
case 6 :  goto oct6;
case 7 :  goto oct7;
case 8 :  goto oct8;
}



oct1:     while(x!=y){
       int e1=sqr(x)+sqr(y+1)-r2;
       int e2=sqr(x-1)+sqr(y+1)-r2;
        if (e1<=e2) {
            y++;
            stepy(1);
            wait(m);
        }
        else {
            y++;
            x--;
            stepy(1);
            stepx(-1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

     }
  //
oct2:  while ((sqr(y)!=r2) && (x!=0)){
        int e1=sqr(x-1)+sqr(y)-r2;
        int e2=sqr(x-1)+sqr(y+1)-r2;
        if (e1<=e2) {
            x--;
            stepx(-1);
            wait(m);
        }
        else {
            y++;
            x--;
            stepy(1);
            stepx(-1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}


  }

  //
oct3:  while(-x!=y){
        int e1=sqr(x-1)+sqr(y)-r2;
        int e2=sqr(x-1)+sqr(y-1)-r2;
        if (e1<=e2) {
            x--;
            stepx(-1);
            wait(m);
        }
        else {
            y--;
            x--;
            stepy(-1);
            stepx(-1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

  }
  //
oct4:  while ((sqr(x)!=r2) && (y!=0)){
        int e1=sqr(x)+sqr(y-1)-r2;
        int e2=sqr(x-1)+sqr(y-1)-r2;
        if (e1<=e2) {
            y--;
            stepy(-1);
            wait(m);
        }
        else {
            y--;
            x--;
            stepy(-1);
            stepx(-1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

  }
  //
oct5:  while(x!=y){
        int e1=sqr(x)+sqr(y-1)-r2;
        int e2=sqr(x+1)+sqr(y-1)-r2;
        if (e1<=e2) {
            y--;
            stepy(-1);
            wait(m);
        }
        else {
            y--;
            x++;
            stepy(-1);
            stepx(1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

  }
  //
oct6:  while ((sqr(y)!=r2) && (x!=0)){
        int e1=sqr(x+1)+sqr(y)-r2;
        int e2=sqr(x+1)+sqr(y-1)-r2;
        if (e1<=e2) {
            x++;
            stepx(1);
            wait(m);
        }
        else {
            y--;
            x++;
            stepy(-1);
            stepx(1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

  }
  //
oct7:  while(x!=-y){
        int e1=sqr(x+1)+sqr(y)-r2;
        int e2=sqr(x+1)+sqr(y+1)-r2;
        if (e1<=e2) {
            x++;
            stepx(1);
            wait(m);
        }
        else {
            y++;
            x++;
            stepy(1);
            stepx(1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

  }
  //
oct8:  while ((sqr(x)!=r2) && (y!=0)){
        int e1=sqr(x)+sqr(y+1)-r2;
        int e2=sqr(x+1)+sqr(y+1)-r2;
        if (e1<=e2) {
            y++;
            stepy(1);
            wait(m);
        }
        else {
            y++;
            x++;
            stepy(1);
            stepx(1);
            wait(m);
        }
        if ((x==xf) && (y==yf)) {goto stop;}

  }
goto oct1;


stop:
    //stop



}


void up_z() {
    for(int i=0;i<300;i++) {stepz(1); wait(m);}
// up z
}

void down_z() {
// down z
for(int i=0;i<300;i++) {stepz(-1); wait(m);}
}


 void go_origin() {

 }


void update(unsigned int x,unsigned int y,unsigned int z) {
X=x;
Y=y;
Z=z;
}



void init() {
    go_origin();
    release();

    X=0;
    Y=0;
    Z=0;

}






int parseint(char **buffer) {
    
//cout <<strchr(*buffer,' ')<< endl;
int x=atoi(*buffer);
int a=strlen(*buffer);
int b=strlen(strchr(*buffer,' '));
*buffer=*buffer+a-b+1;
//cout << *buffer<< endl;
return x;


}


void interpret(string buffer) {
    char k=buffer.at(0);
    buffer.erase(0,2);
    
    char *ch;
    ch=(char*)buffer.c_str();
    
    
         
         
    
switch (k) {
    case 'L' : {
                 // L 256 180 623 123
            
                 int x1=parseint(&ch);
                 int y1=parseint(&ch);
                 int x2=parseint(&ch);
                 int y2=parseint(&ch);
                 
                 if (Z) {up_z(); release(); update(X,Y,0);}
                 
                 Line(X,Y,x1,y1);
                 
                 update(x1,y1,Z);
                 down_z();
                 update(X,Y,1);
                 release();
                 
                 Line(x1,y1,x2,y2);
                 update(x2,y2,1);
                 up_z();
                 release();
                 update(x2,y2,0);
                 break;
                  } 
    case 'C' : {
                // C 100 100 20
                int cx=parseint(&ch);
                int cy=parseint(&ch);
                int r=parseint(&ch);
                if (Z) {up_z(); release();}
                //Line(X,Y,cx+r,cy);
                release();
                down_z();
                Circle(cx,cy,r);
                release();
                up_z();
                release();
                update(cx+r,cy,0);
                break;
                  } 
    case 'P' : { break;

                  } 
    case 'A' : {
                  break;
                  } 
    case 'f' : {
                for(int i=0;i<2000;i++) {stepx(1); wait(m);}
                 release(); 
                 //ser.putc('m');
                 break;
                 } 
        
    case 'g' : {
                for(int i=0;i<2000;i++) {stepx(-1); wait(m);}
                 release(); 
                 break;
                 } 
    case 'h' : {
                for(int i=0;i<4000;i++) {stepy(1); wait(m);}
                 release();
                 break;
                  } 
    case 'j' : {
                for(int i=0;i<4000;i++) {stepy(-1); wait(m);}
                 release();
                 break;
                  } 
    case 'k' : {
                 for(int i=0;i<100;i++) {stepz(-1); wait(m);}
                 release();
                      break;}
    case 'l' : {
                      for(int i=0;i<100;i++) {stepz(1); wait(m);}
                       release();
                      break;}
    case 'm' : {
                      Circle(0,0,50);
                      break;}
    case 'w' : { Line(X,Y,0,0); release(); break;}
     }

}




 int main () {
init();
char c;
buffer="";
while(1)
    {
        if (ser.readable()) {
        c = ser.getc();
        ser.putc(c);
        buffer+=c;
        if (c=='\n'){
        interpret(buffer);
        buffer="";
        ser.putc('>'); //ready for next command

       }


 }
 }

 }



