#ifndef _MARCHINGINFO_H
#define _MARCHINGINFO_H

class MarchingInfo {
  public:
    void nextCell() {      
      if (tx < ty && tx < tz) 
        i += sgx, tmin = tx, tx += dtx, hit_normal.Set(1,0.8,0.8);
      else if (ty < tx && ty < tz)
        j += sgy, tmin = ty, ty += dty, hit_normal.Set(0.8,1,0.8);
      else if (tz < ty && tz < tx)
        k += sgz, tmin = tz, tz += dtz, hit_normal.Set(0.8,0.8,1);
      else if (tx == ty && tx < tz)
        i += sgx, j += sgy, tmin = tx, tx += dtx, ty += dty;
      else if (ty == tz && ty < tx)
        j += sgy, k += sgz, tmin = ty, ty += dty, tz += dtz;
      else if (tz == tx && tz < ty)
        k += sgz, i += sgx, tmin = tz, tz += dtz, tx += dtx;
      else
        i += sgx, j += sgy, k += sgz, tmin = tx, tx += dtx, ty += dty, tz += dtz; 
      
    }
    
    Vec3f getColor() {
      return Vec3f((i*i*i+j*j+k*2+23)%17/20.0, 
                   (j*j*j+k*k+i*7+17)%13/20.0, 
                   (k*k*k+i*i+j*5+31)%19/20.0);
    }

    float tmin;
    Vec3f hit_normal;
    int     i,   j,   k;
    float  tx,  ty,  tz;
    float dtx, dty, dtz;
    int   sgx, sgy, sgz;
};

#endif