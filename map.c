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
#include "rotate.h"
#define M_PI 3.14159265358979323846f
const char *mapa_txt = "mapa_costa_rica.txt";

const int WIDTH = 800;
const int HEIGHT = 600;

Country *mapa_lineas = NULL;
const Country *mapa_original = NULL;

int mapa_actual = 1;

double angulo_actual = 0.0;


const double X_MIN_INIT = -86.0f;
const double X_MAX_INIT = -82.5f;
const double Y_MIN_INIT = 8.0f;
const double Y_MAX_INIT = 11.3f;

double X_MIN = X_MIN_INIT;
double X_MAX = X_MAX_INIT;
double Y_MIN = Y_MIN_INIT;
double Y_MAX = Y_MAX_INIT;

int univ_to_fb_x(double x) {
	return (int)(((x - X_MIN) / (X_MAX - X_MIN)) * WIDTH);
}

int univ_to_fb_y(double y){
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

void free_country(Country *cr) {
    if (!cr) return;

    if (cr->prov) {
        for (int p = 0; p < cr->Q_prov; p++) {
            Province *prov = &cr->prov[p];
            if (prov->polygons) {
                for (int g = 0; g < prov->Q_poly; g++) {
                    Polygon *poly = &prov->polygons[g];
                    if (poly->points) {
                        free(poly->points);
                        poly->points = NULL;
                    }
                }

                free(prov->polygons);
                prov->polygons = NULL;
            }
        }
        free(cr->prov);
        cr->prov = NULL;
    }
    free(cr);
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
				fscanf(map, "%lf %lf", &poly->points[i].x, &poly->points[i].y);
				poly->points[i].w = 1.0f; //Para operaciones de matrices
			}
		}
	}
	fclose(map);
	return cr;	
}


Country* copiar_country(const Country *origen) {
    if (!origen) return NULL;

    Country *copia = malloc(sizeof(Country));
    copia->Q_prov = origen->Q_prov;
    copia->prov = malloc(sizeof(Province) * copia->Q_prov);

    for (int p = 0; p < origen->Q_prov; p++) {
        copia->prov[p].Q_poly = origen->prov[p].Q_poly;
        copia->prov[p].polygons = malloc(sizeof(Polygon) * copia->prov[p].Q_poly);

        for (int g = 0; g < origen->prov[p].Q_poly; g++) {
            copia->prov[p].polygons[g].Q_point = origen->prov[p].polygons[g].Q_point;
            int q_pts = copia->prov[p].polygons[g].Q_point;

            copia->prov[p].polygons[g].points = malloc(sizeof(Point) * q_pts);

            for (int i = 0; i < q_pts; i++) {
                copia->prov[p].polygons[g].points[i] = origen->prov[p].polygons[g].points[i];
            }
        }
    }
    return copia;
}

void keyboard(unsigned char key, int x, int y) {
	(void)x;
	(void)y;

	int modificadores = glutGetModifiers();
	double zoom_speed = (modificadores & GLUT_ACTIVE_CTRL) ? ZOOM_LENTO : ZOOM_NORMAL;
	double rotate_speed = (modificadores & GLUT_ACTIVE_CTRL) ? ROTATE_LENTO : ROTATE_NORMAL;
	double rotate_speed_C = (modificadores & GLUT_ACTIVE_CTRL) ? ROTATE_LENTO_C : ROTATE_NORMAL_C;
	switch (key) {
		case '=':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, 1.0f / zoom_speed);
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case '+':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, 1.0f / ZOOM_RAPIDO);
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case '-':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, zoom_speed);
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case '_':
			zoom(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, ZOOM_RAPIDO);
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case ';':
			angulo_actual += rotate_speed;
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case ':':
			angulo_actual += ROTATE_RAPIDO;
			rotate(mapa_original, mapa_lineas,angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case '.':
			angulo_actual += rotate_speed_C;
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case '>':
			angulo_actual += ROTATE_RAPIDO_C;
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
			break;
		case 'r':
		case 'R':
			angulo_actual = 0.0;
			reset(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, X_MIN_INIT, X_MAX_INIT, Y_MIN_INIT, Y_MAX_INIT);
			rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
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
	double pan_speed = PAN_NORMAL;
	if (modificadores & GLUT_ACTIVE_SHIFT) {
		pan_speed = PAN_RAPIDO;
	} else if (modificadores & GLUT_ACTIVE_CTRL) {
		pan_speed = PAN_LENTO;
	}
	
	double step_x = (X_MAX - X_MIN) * pan_speed;
	double step_y = (Y_MAX - Y_MIN) * pan_speed;
	
	double dir_x = 0.0f;
	double dir_y = 0.0f;

	switch (key) {
		case GLUT_KEY_RIGHT:
			dir_x = step_x;
			break;
		case GLUT_KEY_LEFT:
			dir_x = -step_x;
			break;
		case GLUT_KEY_UP:
			dir_y = step_y;
			break;
		case GLUT_KEY_DOWN:
			dir_y = -step_y;
			break;
		default:
			return;
	}
	double rad = -angulo_actual * (M_PI / 180.0f);
	double cos_a = cosf(rad);
	double sin_a = sinf(rad);
	
	double dx = dir_x * cos_a - dir_y * sin_a;
    double dy = dir_x * sin_a + dir_y * cos_a;
	pan(&X_MIN, &X_MAX, &Y_MIN, &Y_MAX, dx, dy);
	
	rotate(mapa_original, mapa_lineas, angulo_actual, X_MIN, X_MAX, Y_MIN, Y_MAX);
	glutPostRedisplay();
}

void init() {
	glClearColor(0.0,0.0,0.0,1.0);
	glColor3f(1.0f,1.0f,1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,WIDTH,0,HEIGHT);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    render_country(mapa_lineas);
    glFlush();
}

int main(int argc, char **argv) {
	mapa_original = load_map();
	mapa_lineas = copiar_country(mapa_original);
	
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
	free_country(mapa_original);
	free_country(mapa_lineas);
	mapa_original = NULL;
	mapa_lineas = NULL;
	return 0;
}



