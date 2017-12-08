/***********************
   publisher.cpp 
   publisher logic
   for homework project

   zhang chen hui
   asterix314@163.com
***********************/

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <thread>
#include <iostream>
#include "buffer.hpp"

using namespace boost::interprocess;

int main(int argc, char *argv[]) {

  int id = std::stoi(argv[1]); // 1 or 2
  std::cout << "[P" << id << "] starts." << std::endl;
  
  // subscriber 1 should have set up the "homework" shared buffer already.
  // map it to own address space.
  shared_memory_object shm (open_only, "homework", read_write);
  mapped_region mreg(shm, read_write);
  shm_buffer * buf = static_cast<shm_buffer*>(mreg.get_address());
  std::cout << "[P" << id << "] opened shared buffer '" << shm.get_name()
              << "' at " << buf << std::endl;

  // publish data
  std::chrono::milliseconds ms(1);
  std::cout << "[P" << id << "] sending " << num_messages << " messages ..." << std::endl;
  for(int i = 0; i < num_messages; ++i) {
    buf->write(i + (id - 1) * num_messages);
    std::this_thread::sleep_for(ms);  // 1000 messages per second
  }

  std::cout << "[P" << id << "] exit." << std::endl;
  return 0;
}
