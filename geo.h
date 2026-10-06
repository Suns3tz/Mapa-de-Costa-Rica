#ifndef GEO_H
#define GEO_H

typedef struct {
	double x;
	double y;
	double w;
} Point;

typedef struct {
	Point *points;
	int Q_point;
} Polygon;

typedef struct {
	Polygon *polygons;
	int Q_poly;
} Province;

typedef struct {
	Province *prov;
	int Q_prov;
} Country;


#endif
