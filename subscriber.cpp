#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <vector>
#include "homework.hpp"

// todo: make subcriber to create shared buffer and block.

using namespace boost::interprocess;

int main(int argc, char *argv[]) {

  int subscriber_id = std::stoi(argv[1]); // 1
  std::cout << "[s" << subscriber_id << "] starts." << std::endl;

  // to remove shared memory on destruction
  struct shm_remove  {
    ~shm_remove(){
      shared_memory_object::remove("homework");
    }
  } shm_remover;

  // set up shared memory to hold the shm_buffer
  // publisher 1 should've placed the structure there
  shared_memory_object shm (open_only, "homework", read_write);
  mapped_region region(shm, read_write);
  shm_buffer * buffer = static_cast<shm_buffer*>(region.get_address());
  std::cout << "[s" << subscriber_id << "] opened shared buffer." << std::endl;

  std::vector<check_record> checks(NUM_MESSAGES * NUM_PUBLISHERS);

  for(auto &check: checks) {
    check = buffer->pop();
    std::cout << "elapsed: " << check.elapsed_us <<
      "us. data = " << check.data << std::endl;
  }

  std::cout << "[s" << subscriber_id << "] remove shared buffer." << std::endl;
  return 0;
}
