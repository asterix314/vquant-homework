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
#include "buffer.hpp"

using namespace boost::interprocess;
using namespace boost::accumulators;

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

  // check received results
  std::array<check_record, NUM_MESSAGES * NUM_PUBLISHERS> checks;
  
  // accumulator for statistics: min, max, mean, variance, etc.
  accumulator_set<
    double,
    stats<tag::min, tag::max, tag::mean, tag::variance>
    > acc_stats;

  for(auto &chk: checks) {
    chk = buf->pop();
    acc_stats(chk.elapsed_us);
  }

  // sort the checks array by .data.
  std::sort(checks.begin(), checks.end(),
            [](check_record &a, check_record &b) {
              return a.data < b.data;
            });

  bool rcvd_ok = true;
  for(int i=0; i<NUM_MESSAGES * NUM_PUBLISHERS; ++i) {
     rcvd_ok = rcvd_ok && (i == checks[i].data);
  }
  
  if (rcvd_ok) {
    std::cout << "[S" << id << "] check received messages: ok." << std::endl;
  } else {
    std::cout << "[S" << id << "] ERROR found in received messages." << std::endl;
  }

  std::cout << "[S" << id << "] statistics of delays in micro seconds:" << std::endl;
  std::cout << "min = " << min(acc_stats) << " max = " << max(acc_stats)
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
