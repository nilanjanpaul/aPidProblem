#ifndef _CUTIL_PID_H_
#define _CUTIL_PID_H_

#include <iostream>
#include <string.h>
#include <vector>


/**
 * Object containing 
 * 
 *
 */

class CUtil_PID
{

 private:
  float _kp;
  float _ki;
  float _kd;
  
  float _last_error;
  float _sum_error;
  float _delta_error;
  float _current_pv;

  float _limit_max;
  float _limit_min;

  
 public:

  // Constructor
  CUtil_PID();

 ~CUtil_PID();


 // handlers for gain
 void  set_kp(float v) { _kp = v; }
 void  set_ki(float v) { _ki = v; }
 void  set_kd(float v) { _kd = v; }
 
 void init();
 
 float calc_plant_output(float sv, float pv);


 void debug();
};

#endif
