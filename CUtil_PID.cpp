#include "CUtil_PID.h"
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <assert.h>



/**
 * Constructor
 * init
 *
 */
CUtil_PID::CUtil_PID()
{
    
}

/**
 * Destructor
 * -- Free up memory
 *
 */
CUtil_PID::~CUtil_PID()
{

}

void
CUtil_PID::init()
{
  _last_error = 0.0;
  _sum_error = 0.0;
  _current_pv = 0.0;
  _delta_error = 0.0;

  _limit_max = 1000.0;
  _limit_min = -1.0;
  
}

float
CUtil_PID::calc_plant_output(float sv, float pv)
{

  // calculate errors
  float error = sv - pv;  // proportional error
  _sum_error += error * 1.0; // sum error
  _delta_error = (error - _last_error); // difference error

  // calc pid terms and output
  float p1 = _kp * error;   // proportional term
  float p2 = _ki * _sum_error; // integral term
  float p3 = _kd * _delta_error; // 0.0  // derivative term

  _current_pv = p1 + p2 + p3;

  // check limits for output
  std::min(_current_pv, _limit_max);
  std::max(_current_pv, _limit_min);


  _last_error = error;

  
  return _current_pv;
}


/**
 * debug()
 * -- writes out time LLA points to std cout
 *
 */
void
CUtil_PID::debug()
{
  static bool bSchema = false;

  if (!bSchema) {
    std::cerr << "| _last_error | _sum_error | _delta_error | _kp | _ki | kd | _current_pv | " << std::endl;
    bSchema = true;
  }
    
  std::cerr << " | " << _last_error;
  std::cerr << " | " << _sum_error;
  std::cerr << " | " << _delta_error;

  std::cerr << " | " << _kp;
  std::cerr << " | " << _ki;
  std::cerr << " | " << _kd;
  
  std::cerr << " | " << _current_pv;
  std::cerr << std::endl;
  
  return;
}
