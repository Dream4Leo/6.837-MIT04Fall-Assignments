#ifndef _MARCHINGINFO_H
#define _MARCHINGINFO_H


class MarchingInfo {
  public:
    void nextCell() {
      if (tx <= ty && tx <= tz) 
        i += sgx, tmin = tx, tx += dtx, hit_normal.Set(1,0.8,0.8);
      else if (ty <= tx && ty <= tz)
        j += sgy, tmin = ty, ty += dty, hit_normal.Set(0.8,1,0.8);
      else
        k += sgz, tmin = tz, tz += dtz, hit_normal.Set(0.8,0.8,1);
    }
    
    Vec3f getColor() {
      return Vec3f((i*i*i+j*j+k*2+15)%17/20.0, 
                   (j*j*j+k*k+i*3+11)%13/20.0, 
                   (k*k*k+i*i+j*7+13)%19/20.0);
    }

    float tmin;
    Vec3f hit_normal;
    int     i,   j,   k;
    float  tx,  ty,  tz;
    float dtx, dty, dtz;
    int   sgx, sgy, sgz;
};


#endif