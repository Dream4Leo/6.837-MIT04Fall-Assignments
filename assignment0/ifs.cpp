#include <stdio.h>
#include <stdlib.h>

#include "image.cpp"
#include "matrix.cpp" 
#include "ifs.h"


void Ifs::Render(int points, int iters, int size, const char *filename) {
	
	int x, y;
	float vx, vy;
	Image image(size, size);
	
	for (int i = 0; i < points; ++i) {
		x = rand() % size;
		y = rand() % size;
		Vec3f v(1.0*x/size, 1.0*y/size, 0.0);
		for (int j = 0; j < iters; ++j) {
			Matrix* mat = RandPickMat();
			mat->Transform(v);
			v.Clamp();
		}
		x = v.x() * size;
		y = v.y() * size;
		Vec3f color(1.0, 1.0, 1.0);
		image.SetPixel(x, y, color);
	}
	image.SaveTGA(filename);
}

Matrix* Ifs::RandPickMat() {
	float p = rand() % 100 / 100.0;
	float sum = 0.0;
	for (int i = 0; i < n; i++)
	{
		sum += prob[i];
		if ( sum > p ) return &(mats[i]);
	}
}
