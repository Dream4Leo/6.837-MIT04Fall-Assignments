#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ray.h"
#include "hit.h"
#include "grid.h"
#include "boundingbox.h"


class Object3D {

  public:

    Object3D() {}
    ~Object3D() {}
    
    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual void insertIntoGrid(Grid *g, Matrix *m) {}
    BoundingBox* getBBox() { return bbox; }
    
  protected:
    BoundingBox *bbox;
    Material *type;
};


#endif