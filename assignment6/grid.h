#ifndef _GRID_H
#define _GRID_H

#include <math.h>
#include "boundingbox.h"
#include "marchinginfo.h"
#include "object3dvector.h"

class Grid {

  public:
    Grid(BoundingBox *b, int _nx, int _ny, int _nz): 
      nx(_nx), ny(_ny), nz(_nz) {
      bbox = b;
      Vec3f min(b->getMin()), max(b->getMax());
      vx = (max.x()-min.x()) / nx;
      vy = (max.y()-min.y()) / ny;
      vz = (max.z()-min.z()) / nz;
      voxel = new Object3DVector**[nx];
      for(int i = 0; i < nx; i++) {
        voxel[i] = new Object3DVector*[ny];
        for(int j = 0; j < ny; j++) {
          voxel[i][j] = new Object3DVector[nz];
        }
      }
    }
    ~Grid() {
      for(int i = 0; i < nx; i++) {
        for(int j = 0; j < ny; j++) delete [] voxel[i][j];
        delete [] voxel[i];
      }
    }
    // ACCESSORS
    int getNx() { return nx; }
    int getNy() { return ny; }
    int getNz() { return nz; }
    float getVx() { return vx; }
    float getVy() { return vy; }
    float getVz() { return vz; }
    BoundingBox *getBBox() { return bbox; }
    Object3DVector *getVoxel(int i, int j, int k) { return &voxel[i][j][k]; }
    Object3DVector *getNextVoxel(MarchingInfo &mi);

    void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin);

    // HELP FUNCTIONS
    bool insideGrid(Vec3f &p);
    bool insideVoxel(int i, int j, int k, Vec3f p);
    Vec3f getVoxelPos(int i, int j, int k);
    void  getVoxelID(Vec3f min, Vec3f max, Vec3f &l, Vec3f &r);
    float getPlaneIntersection(const Ray &r, Vec3f &normal, float offset, float tmin);
    float getBBoxIntersection(const Ray &r, MarchingInfo &mi, float tmin);

  protected:
    Object3DVector*** voxel;
    BoundingBox *bbox;
    int nx, ny, nz;
    float vx, vy, vz;
};

bool Grid::insideGrid(Vec3f &p) {
  Vec3f min(bbox->getMin());
  Vec3f max(bbox->getMax());
  return min.x() <= p.x() && p.x() <= max.x()
      && min.y() <= p.y() && p.y() <= max.y()
      && min.z() <= p.z() && p.z() <= max.z();
}

bool Grid::insideVoxel(int i, int j, int k, Vec3f p) {
  p -= bbox->getMin();
  float eps = 1e-1;
  return i * vx - eps <= p.x() && p.x() <= (i+1) * vx + eps 
      && j * vy - eps <= p.y() && p.y() <= (j+1) * vy + eps
      && k * vz - eps <= p.z() && p.z() <= (k+1) * vz + eps;
}
   
void Grid::getVoxelID(Vec3f bmin, Vec3f bmax, Vec3f &l, Vec3f &r) {
  Vec3f b = bbox->getMin();

  bmin -= b; 
  int lx = max(min(int((bmin.x())/vx), nx), 0);
  int ly = max(min(int((bmin.y())/vy), ny), 0);
  int lz = max(min(int((bmin.z())/vz), nz), 0);

  bmax -= b;
  int rx = max(min(int((bmax.x())/vx), nx), 0);
  int ry = max(min(int((bmax.y())/vy), ny), 0);
  int rz = max(min(int((bmax.z())/vz), nz), 0);
  
  //Edge/Boundary cases
  rx = rx - (rx > lx && fabs(bmax.x() - rx * vx) < 1e-5);
  ry = ry - (ry > ly && fabs(bmax.y() - ry * vy) < 1e-5);
  rz = rz - (rz > lz && fabs(bmax.z() - rz * vz) < 1e-5);
  if (lx == rx && lx == nx) --lx, --rx;
  if (ly == ry && ly == ny) --ly, --ry;
  if (lz == rz && lz == nz) --lz, --rz;

  l.Set(lx, ly, lz);
  r.Set(rx, ry, rz);
}

Vec3f Grid::getVoxelPos(int i, int j, int k) {
  Vec3f p(bbox->getMin());
  return Vec3f(p.x()+(i+0.5)*vx, p.y()+(j+0.5)*vy, p.z()+(k+0.5)*vz);
}

float Grid::getPlaneIntersection(const Ray &r, Vec3f &normal, float offset, float tmin) {
  float theta = normal.Dot3(r.getDirection());
  return theta == 0.0 ? 1e8 : -(-offset + normal.Dot3(r.getOrigin())) / theta;
}

float Grid::getBBoxIntersection(const Ray &r, MarchingInfo &mi, float tmin) {
  
  Vec3f bmin(bbox->getMin()), bmax(bbox->getMax());
  Vec3f dir(r.getDirection());
  
  // check parallelism
  Vec3f org(r.getOrigin());
  bool xparal = dir.x() == 0.0 && (bmin.x() > org.x() || org.x() > bmax.x());
  bool yparal = dir.y() == 0.0 && (bmin.y() > org.y() || org.y() > bmax.y());
  bool zparal = dir.z() == 0.0 && (bmin.z() > org.z() || org.z() > bmax.z());
  if (xparal || yparal || zparal) return 0.0;

  //intersection with x/y/z-near/far plane
  float t[6];
  Vec3f normx(1,0,0), normy(0,1,0), normz(0,0,1);
  t[0] = getPlaneIntersection(r, normx, bmin.x(), tmin);
  t[1] = getPlaneIntersection(r, normx, bmax.x(), tmin);
  t[2] = getPlaneIntersection(r, normy, bmin.y(), tmin);
  t[3] = getPlaneIntersection(r, normy, bmax.y(), tmin);
  t[4] = getPlaneIntersection(r, normz, bmin.z(), tmin);
  t[5] = getPlaneIntersection(r, normz, bmax.z(), tmin);
  if (dir.x() < 0.0) swap(t[0], t[1]);
  if (dir.y() < 0.0) swap(t[2], t[3]);
  if (dir.z() < 0.0) swap(t[4], t[5]);
  if (dir.x() == 0.0) t[0] = -1e8;
  if (dir.y() == 0.0) t[2] = -1e8;
  if (dir.z() == 0.0) t[4] = -1e8;

  //interstion with boundingbox
  float intt = max(max(t[0],t[2]),t[4]);
  float outt = min(min(t[1],t[3]),t[5]);
  if (intt <= outt && intt > tmin) {
    if (intt == t[0]) mi.hit_normal = normx; else
    if (intt == t[2]) mi.hit_normal = normy; else
                      mi.hit_normal = normz;
    return intt;
  } else
    return 0.0;
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) {
  
  Vec3f p(r.pointAtParameter(tmin));
  Vec3f bmin(bbox->getMin());
  Vec3f bmax(bbox->getMax());

  if (insideGrid(p))  //Case 1: ray starts inside grid
    mi.tmin = tmin;   // does not seem correct
  else {              //Case 23:ray starts outside grid
    mi.tmin = getBBoxIntersection(r, mi, tmin);
    if (mi.tmin == 0.0) return; // ray does not intersect with grid
    p = r.pointAtParameter(mi.tmin);
  }

  Vec3f dir(r.getDirection()); dir.Normalize();
  mi.sgx = dir.x() > 0 ? 1 : -1;
  mi.sgy = dir.y() > 0 ? 1 : -1; 
  mi.sgz = dir.z() > 0 ? 1 : -1;
  
  p -= bmin;
  mi.i = max(min(int(p.x()/vx), nx-1), 0);
  mi.j = max(min(int(p.y()/vy), ny-1), 0);
  mi.k = max(min(int(p.z()/vz), nz-1), 0);
  if (mi.i > 0 && dir.x() < 0 && mi.i * vx == p.x()) --mi.i;
  if (mi.j > 0 && dir.y() < 0 && mi.j * vy == p.y()) --mi.j;
  if (mi.k > 0 && dir.z() < 0 && mi.k * vz == p.z()) --mi.k;

  mi.dtx = dir.x() == 0 || vx == 0 ? 1e8 : vx / fabs(dir.x());
  mi.dty = dir.y() == 0 || vy == 0 ? 1e8 : vy / fabs(dir.y());
  mi.dtz = dir.z() == 0 || vz == 0 ? 1e8 : vz / fabs(dir.z());

  Vec3f normx(1,0,0), normy(0,1,0), normz(0,0,1);
  mi.tx = getPlaneIntersection(r, normx, bmin.x()+(mi.i+(dir.x()>0))*vx, tmin);
  mi.ty = getPlaneIntersection(r, normy, bmin.y()+(mi.j+(dir.y()>0))*vy, tmin);
  mi.tz = getPlaneIntersection(r, normz, bmin.z()+(mi.k+(dir.z()>0))*vz, tmin);
}

Object3DVector* Grid::getNextVoxel(MarchingInfo &mi) {
  mi.nextCell();
  if (0 <= mi.i && mi.i < nx && 
      0 <= mi.j && mi.j < ny &&
      0 <= mi.k && mi.k < nz)
    return getVoxel(mi.i, mi.j, mi.k);
  else 
    return NULL;
}

#endif