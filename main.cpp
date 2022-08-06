#include <iostream>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

#include <signal.h>
#include "CUtil_PID.h"

#define DOUT(x) std::cerr << #x << " = " << std::setprecision(20) << x << std::setprecision(6) << std::endl


namespace po = boost::program_options;

void signal_interrupt_control_c(int s){
  printf("Caught signal %d\n",s);
}


int main(int argc, char *argv[]){

  float kp;
  float ki;
  float kd;
  float set_value;
  float assert_value;
  unsigned int iterations;
  
  //setup the program options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "help message")
    ("kp",   po::value<float>(&kp)->default_value(1.0),"proportional gain")
    ("ki",   po::value<float>(&ki)->default_value(1.0),"integral gain")
    ("kd",   po::value<float>(&kd)->default_value(1.0),"derivative gain")
    ("sv",   po::value<float>(&set_value)->default_value(4.2),"set point - controller input")
    ("iter", po::value<unsigned int>(&iterations)->default_value(40), "number of successive loops to iterate pid kernel")
    ("assert_value",   po::value<float>(&assert_value)->default_value(4.2),"plant value after specificed iterations")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  //print the help message
  if (vm.count("help")){
    std::cout << desc << std::endl << std::endl;
    std::cout << "Example usage with options (aka test cases):\n";
    std::cout << boost::format("    %s --kp 1.1 --ki 0.2 --kd .02 --sv 4.2 --iter 38 --assert_value -10530.966796875\n") % argv[0];
    std::cout << boost::format("    %s --kp 0.1 --ki 0.2 --kd .02 --sv 4.2 --iter 400 --assert_value 4.2\n") % argv[0];
    std::cout << boost::format("    %s --kp 0.125 --ki 0.25 --kd .0625 --sv 4.2 --iter 80 --assert_value 4.2\n") % argv[0];
    std::cout << boost::format("\n");
    return 0;
  }

  // Ctrl-C for breaking
  signal (SIGINT, signal_interrupt_control_c);



  CUtil_PID pid;
  pid.init();

  // Set PID gains
  
  pid.set_kp( kp );
  pid.set_ki( ki );
  pid.set_kd( kd );
  
 
  // initial plant value
  float plant_value = 0.0;

  for (int i = 0; i < iterations; ++i) {  

    // execute pid kernel
    plant_value = pid.calc_plant_output(set_value, plant_value);
    pid.debug();

  }

  const float EPSILON = 1e-6;
  //DOUT(assert_value);
  //DOUT(plant_value);
  //DOUT(assert_value - plant_value);
 
  assert(std::fabs(assert_value - plant_value) < EPSILON);
  

  std::cout << std::endl << "TEST PASSED!!" << std::endl;
  std::cout << "Assert value and plant value matched: " << assert_value << " , " << plant_value <<  std::endl;    

  std::cout << std::endl << "Done!" << std::endl;    
  return EXIT_SUCCESS;
}
