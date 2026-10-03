#include <stdlib.h>
#include <math.h>
#include "bresenham.h"
#include <stdio.h>
#include <GL/gl.h>


void plot(int col, int row)
{
	
    glVertex2f(
        (float)col + 0.5f,
        (float)row + 0.5f
    );
    
}

int classify_octant(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
	
    // Si solo es un punto se asigna al octante 1
    if (dx == 0 && dy == 0) {
        return 1;
    }

    // En la mitad derecha x aumenta
    if (dx >= 0) {
        if (dy >= 0) {
            if (dx >= dy) {
                return 1;
            }

            return 2;
        }
        if (dx >= -dy) {
            return 8;
        }

        return 7;
    }

    // En la mitad izquierda x disminuye
    if (dy >= 0) {
        if (dy >= -dx) {
            return 3;
        }

        return 4;
    }
    if (-dx >= -dy) {
        return 5;
    }

    return 6;
}

// Bresenham en C
void oct_1(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y1 - y0);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  plot(xp,yp);
  
  int d = 2* (y1-y0) - (x1 - x0);
  while(xp < x1){
    if(d<=0){
      xp++;
      d = d + Delta_E;
    }else{
      xp++;
      yp++;
      d = d + Delta_Ne;
    }
    plot(xp,yp);
  } 
}

void oct_2(int x0, int y0, int x1, int y1){
  int Delta_N = 2*(x0 - x1);
  int Delta_Ne = 2*((y1 - y0) - (x1 - x0));

  int xp = x0;
  int yp = y0;
  plot(xp,yp);

  int d = (y1-y0) - 2*(x1 - x0);
  while(yp < y1){
    if(d<=0){
      xp++;
      yp++;
      d = d + Delta_Ne;
    }else{
      yp++;
      d = d + Delta_N;
    }
    plot(xp,yp);
  }
}

void oct_3(int x0, int y0, int x1, int y1){
  int Delta_N = 2*(x0 - x1);
  int Delta_NO = 2*((y0 - y1) + (x0 - x1));

  int xp = x0;
  int yp = y0;
  plot(xp,yp);

  int d = (y0-y1) - 2*(x1 - x0);
  while(yp < y1){
    if(d<=0){
      yp++;
      d = d + Delta_N;
    }else{
      xp--;
      yp++;
      d = d + Delta_NO;
    }
    plot(xp,yp);
  }
}

void oct_4(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y0 - y1);
  int Delta_NO = 2*((y0 - y1) + (x0 - x1));

  int xp = x0;
  int yp = y0;
  plot(xp,yp);

  int d = 2*(y0-y1) - (x1 - x0);
  while(xp > x1){
    if(d<=0){
      xp--;
      yp++;
      d = d + Delta_NO;
    }else{
      xp--;
      d = d + Delta_O;
    }
    plot(xp,yp);
  }
}

void oct_5(int x0, int y0, int x1, int y1){
  int Delta_O = 2*(y1 - y0);
  int Delta_SO =  2*((y1 - y0) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;
  plot(xp,yp);
  
  int d = 2 * (y1-y0) - (x1 - x0);
  while(xp > x1){
    if(d>=0){
      xp--;
      d = d + Delta_O;
    }else{
      xp--;
      yp--;
      d = d + Delta_SO;
    }
    plot(xp,yp);
  } 
}

void oct_6(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x0 - x1);
  int Delta_SO =  2*((x0 - x1) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  plot(xp,yp);
  
  int d = 2 * (x0 - x1) - (y0-y1);
  while(yp > y1){
    if(d>=0){
      yp--;
      xp--;
      d = d + Delta_SO;
    }else{
      yp--;
      d = d + Delta_S;
    }
    plot(xp,yp);
  } 
}

void oct_7(int x0, int y0, int x1, int y1){
  int Delta_S = 2*(x1-x0);
  int Delta_SE = 2*((x1-x0) - (y0 - y1));
  
  int xp = x0;
  int yp = y0;
  
  plot(xp,yp);

  int d = 2 * (x1 - x0) - (y0-y1);
  while(yp > y1){
    if(d>=0){
      yp--;
      xp++;
      d = d + Delta_SE;
    }else{
      yp--;
      d = d + Delta_S;
    }
    plot(xp,yp);
  } 
}

void oct_8(int x0, int y0, int x1, int y1){
  int Delta_E = 2*(y0 - y1);
  int Delta_SE = 2*((y0-y1) - (x1 - x0));
  
  int xp = x0;
  int yp = y0;

  plot(xp,yp);

  int d = 2 * (y0-y1) - (x1 - x0);

  while(xp < x1){
    if(d <= 0){
      xp++;
      d = d + Delta_E;
    }else{
      xp++;
      yp--;
      d = d + Delta_SE;
    }
    plot(xp,yp);
  } 
}

int BresenhamC(int x0, int y0, int x1, int y1) {
    int octant = classify_octant(x0, y0, x1, y1);
	
    switch (octant) {
        case 1:
            oct_1(x0, y0, x1, y1);
            return 1;

        case 2:
            oct_2(x0, y0, x1, y1);
            return 1;

        case 3:
            oct_3(x0, y0, x1, y1);
            return 1;

        case 4:
            oct_4(x0, y0, x1, y1);
            return 1;

        case 5:
            oct_5(x0, y0, x1, y1);
            return 1;
        
        case 6:
            oct_6(x0, y0, x1, y1);
            return 1;
        
        case 7:
            oct_7(x0, y0, x1, y1);
            return 1;

        case 8:
            oct_8(x0, y0, x1, y1);
            return 1;

        default:
            return 0;
    }
}
