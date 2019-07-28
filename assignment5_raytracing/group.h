#ifndef _GROUP_H
#define _GROUP_H

#include <algorithm>
#include "object3d.h"

class Group: public Object3D {

  public:

    Group(int n): num(n) { 
      instances = new Object3D *[n]; 
      bbox = new BoundingBox(Vec3f(1e8,1e8,1e8), Vec3f(-1e8,-1e8,-1e8));
    }
    ~Group() { delete [] instances; }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) {
      bool intersected = 0;
      for (int i = 0; i < num; ++i)
        intersected |= instances[i]->intersect(r, h, tmin);
      return intersected;
    }
    
    void addObject(int index, Object3D *obj) {
      assert(index >=0 && index < num);
      instances[index] = obj;
      if (obj->getBBox() != NULL)
        bbox->Extend(obj->getBBox());
    }

    void insertIntoGrid(Grid *g, Matrix *m) {
      for (int i = 0; i < num; ++i)
        if (instances[i]->getBBox() != NULL)
          instances[i]->insertIntoGrid(g, m);
    }

  protected:
    int num;
    Object3D** instances;
};

#endif