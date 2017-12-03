#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <thread>
#include <iostream>
#include "homework.hpp"

using namespace boost::interprocess;
using namespace std::chrono_literals;

int main(int argc, char *argv[]) {

  int publisher_id = std::stoi(argv[1]); // 1 or 2
  std::cout << "[p" << publisher_id << "] starts." << std::endl;

  // to remove shared memory on destruction
  struct shm_remove  {
    ~shm_remove(){
      shared_memory_object::remove("homework");
    }
  } shm_remover;
  
  // set up shared memory to hold the shm_buffer
  // 1st publisher constructs the buffer structure for all to use
  shared_memory_object shm (open_or_create, "homework" , read_write);

  if (1 == publisher_id) {
    shm.truncate(sizeof(shm_buffer));
  }

  mapped_region region(shm, read_write);

  shm_buffer *buffer = 0;
  if (1 == publisher_id) {
    buffer = new (region.get_address()) shm_buffer;
    std::cout << "[p" << publisher_id << "] created shared buffer." << std::endl;
  } else {
    buffer = static_cast<shm_buffer*>(region.get_address());
    std::cout << "[p" << publisher_id << "] opened shared buffer." << std::endl;
  }

  // publish data
  std::cout << "[p" << publisher_id << "] publishing data ..." << std::endl;
  for(int i = 0; i < NUM_MESSAGES; ++i) {
    buffer->push(i + (publisher_id - 1) * NUM_MESSAGES);
    std::this_thread::sleep_for(1ms);  // 1000 messages per second
  }

  std::cout << "[p" << publisher_id << "] remove shared buffer." << std::endl;
  return 0;
}
