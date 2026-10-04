#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <unistd.h> 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "bresenham.h"
#include "geo.h"
#include "zoom.h"
#include "pan.h"
#include "reset.h"

const char *mapa_txt = "mapa_costa_rica.txt";

const int WIDTH = 800;
const int HEIGHT = 600;

Country *mapa_lineas = NULL;
Country *mapa_relleno = NULL;
Country *mapa_texturas = NULL;

int mapa_actual = 1;


const float X_MIN_INIT = -86.0f;
const float X_MAX_INIT = -82.5f;
const float Y_MIN_INIT = 8.0f;
const float Y_MAX_INIT = 11.3f;

float X_MIN = X_MIN_INIT;
float X_MAX = X_MAX_INIT;
float Y_MIN = Y_MIN_INIT;
float Y_MAX = Y_MAX_INIT;

int univ_to_fb_x(float x) {
	return (int)(((x - X_MIN) / (X_MAX - X_MIN)) * WIDTH);
}

int univ_to_fb_y(float y){
	return (int)(((y - Y_MIN) / (Y_MAX - Y_MIN)) * HEIGHT);
}

void render_country(const Country *cr) {
    if (!cr) return;
	
	glBegin(GL_POINTS);
	
    for (int p = 0; p < cr->Q_prov; p++) {
        Province *prov = &cr->prov[p];
        
        for (int g = 0; g < prov->Q_poly; g++) {
            Polygon *poly = &prov->polygons[g];
            if (poly->Q_point < 2) continue;
            for (int i = 0; i < poly->Q_point - 1; i++) {
				int x0 = univ_to_fb_x(poly->points[i].x);
				int y0 = univ_to_fb_y(poly->points[i].y);
				int x1 = univ_to_fb_x(poly->points[i+1].x);
				int y1 = univ_to_fb_y(poly->points[i+1].y);
                BresenhamC(x0, y0, x1, y1);
            }
            
            int x_last = univ_to_fb_x(poly->points[poly->Q_point -1].x);
            int y_last = univ_to_fb_y(poly->points[poly->Q_point - 1].y);
            int x_first = univ_to_fb_x(poly->points[0].x);
            int y_first = univ_to_fb_y(poly->points[0].y);
            BresenhamC(x_last, y_last, x_first, y_first);
        }
    }
    glEnd();
}

Country* load_map() {
	FILE *map = fopen(mapa_txt, "r");
	if(!map) {
		printf( "Error abriendo achivo del mapa");
		return NULL;
	} 
	
	Country *cr = malloc(sizeof(Country));
	if (!cr) {
		fclose(map);
		return NULL;
	}
	/** Estructura del txt:
		Numero de Provincias
		Numero de poligonos en la provincia
		Cantidad de puntos por provincia
		Puntos en la provincia (x , y)
		...
	**/
	if (fscanf(map, "%d", &cr->Q_prov) != 1){
		fclose(map);
		return NULL;
	}
	cr->prov = malloc(sizeof(Province) * cr->Q_prov);
	for( int p = 0; p < cr->Q_prov; p++){
		Province *prov = &cr->prov[p];
		fscanf(map, "%d", &prov->Q_poly);
		prov->polygons = malloc(sizeof(Polygon) *prov->Q_poly);
		
		for(int g = 0; g < prov->Q_poly; g++){
			Polygon *poly = &prov->polygons[g];
			fscanf(map, "%d", &poly->Q_point);
			poly->points = malloc(sizeof(Point) * poly->Q_point);
			
			for(int i = 0; i < poly->Q_point; i++){
				fscanf(map, "%f %f", &poly->points[i].x, &poly->points[i].y);
				poly->points[i].w = 1.0f; //Para operaciones de matrices
			}
		}
	}
	fclose(map);
	return cr;	
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	render_country(mapa_lineas);
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	(void)x;
	(void)y;

	int modificadores = glutGetModifiers();
	float zoom_speed = (modificadores & GLUT_ACTIVE_CTRL) ? ZOOM_LENTO : ZOOM_NORMAL;

	switch (key) {
		case '=':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, 1.0f / zoom_speed);
			break;
		case '+':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, 1.0f / ZOOM_RAPIDO);
			break;
		case '-':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, zoom_speed);
			break;
		case '_':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, ZOOM_RAPIDO);
			break;
		case 'r':
		case 'R':
			reset(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, X_MIN_INIT, X_MAX_INIT, Y_MIN_INIT, Y_MAX_INIT);
			break;
		default:
			return;
	}

	glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
	(void)x;
	(void)y;

	int modificadores = glutGetModifiers();
	float pan_speed = PAN_NORMAL;
	if (modificadores & GLUT_ACTIVE_SHIFT) {
		pan_speed = PAN_RAPIDO;
	} else if (modificadores & GLUT_ACTIVE_CTRL) {
		pan_speed = PAN_LENTO;
	}
	float dx = (X_MAX - X_MIN) * pan_speed;
	float dy = (Y_MAX - Y_MIN) * pan_speed;

	switch (key) {
		case GLUT_KEY_RIGHT:
			pan(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, dx, 0.0f);
			break;
		case GLUT_KEY_LEFT:
			pan(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, -dx, 0.0f);
			break;
		case GLUT_KEY_UP:
			pan(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, 0.0f, dy);
			break;
		case GLUT_KEY_DOWN:
			pan(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, 0.0f, -dy);
			break;
		default:
			return;
	}

	glutPostRedisplay();
}

void init() {
	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0f,1.0f,1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,WIDTH,0,HEIGHT);
}

int main(int argc, char **argv) {
	mapa_lineas = load_map();
	if (!mapa_lineas) {
		printf("No se pudo cargar el mapa");
		return 1;
	}
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Mapa de Costa Rica");
	
	init();
	
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutMainLoop();
	
	return 0;
}



