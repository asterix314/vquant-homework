/***********************
   subscriber.cpp 
   subscriber logic
   for homework project

   zhang chen hui
   asterix314@163.com
***********************/

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include "buffer.hpp"

using namespace boost::interprocess;
using namespace boost::accumulators;
using namespace std::chrono;

int main(int argc, char *argv[]) {

  int id = std::stoi(argv[1]); // 1
  std::cout << "[S" << id << "] starts." << std::endl;

  // set up shared memory to hold the buffer structure
  // subscriber 1 constructs the buffer structure for all to use
  shared_memory_object shm (open_or_create, "homework" , read_write);
  if (1 == id) {
    shm.truncate(sizeof(shm_buffer));
    std::cout << "[S" << id << "] size of shared buffer set to "
              << sizeof(shm_buffer) << " bytes." << std::endl;
  }
  mapped_region mreg(shm, read_write);

  shm_buffer *buf = 0;
  if (1 == id) {
    buf = new (mreg.get_address()) shm_buffer;
    std::cout << "[S" << id << "] created shared buffer '" << shm.get_name()
              << "' at " << buf << std::endl;
  } else {
    buf = static_cast<shm_buffer*>(mreg.get_address());
    std::cout << "[S" << id << "] opened shared buffer '" << shm.get_name()
              << "' at " << buf << std::endl;
  }
  
  // receive messages and put them into an array
  std::cout << "[S" << id << "] receiving messages ..." << std::endl;

  struct record {         // for statistics
    double elapsed_ns;    // nanoseconds elapsed
    int data;
  };

  std::array<record, num_messages * num_publishers> checks;
  for(auto &chk: checks) {
    message m = buf->read();
    duration<double, std::nano> elapsed = high_resolution_clock::now() - m.timestamp;
    chk = { elapsed.count(), m.data[0] };
  }

  // accumulator for statistics: min, max, mean, variance, etc.
  accumulator_set<
    double,
    stats<tag::min, tag::max, tag::mean, tag::variance>
    > acc_stats;

  // save delays data to text file
  std::ofstream ostrm("delays.txt");  

  for(auto &chk: checks) {
    acc_stats(chk.elapsed_ns);
    ostrm << chk.elapsed_ns << std::endl;
  }

  // now to check whether we received every message
  // sort the checks array by .data, and it must be increasing.
  std::sort(checks.begin(), checks.end(),
            [](const record &a, const record &b) {
              return a.data < b.data;
            });

  bool data_ok = true;
  for(int i=0; i < num_messages * num_publishers; ++i) {
    data_ok = data_ok && (i == checks[i].data);
  }
  
  if (data_ok) {
    std::cout << "[S" << id << "] check received messages: ok." << std::endl;
  } else {
    std::cout << "[S" << id << "] ERROR found in received messages." << std::endl;
  }

  std::cout << "[S" << id << "] statistics of delays in nano seconds:" << std::endl;
  std::cout << "[S" << id << "] min = " << min(acc_stats) << " max = " << max(acc_stats)
            << " mean = " << mean(acc_stats) << " std = " << std::sqrt(variance(acc_stats))
            << std::endl;

  if (1 == id) {
    shared_memory_object::remove("homework");
    std::cout << "[S" << id << "] remove shared buffer and exit." << std::endl;
  } else {
    std::cout << "[S" << id << "] exit." << std::endl;
  }

  return 0;
}
