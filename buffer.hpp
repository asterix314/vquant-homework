/***********************
   buffer.hpp 
   messages and buffer operations, macro constants.
   for homework project

   zhang chen hui
   asterix314@163.com
***********************/

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <chrono>
#include <atomic>

using namespace std::chrono;
using namespace boost::interprocess;

constexpr int message_size = 256;        // message size in bytes.
constexpr int num_messages = 1000;       // #messages to send per publisher
constexpr int num_publishers = 2;        // #publishers
constexpr int num_subscribers = 1;       // #subscribers
constexpr int buffer_length = 16;        // #messages to hold in buffer

struct message {
  high_resolution_clock::time_point timestamp;
  int data[message_size / sizeof(int)];
};

// shared memory buffer 
struct shm_buffer {
  int wi = 0;           // wirte index
  int ri = 0;           // read index
  message msg[buffer_length];
  std::atomic_bool can_write[buffer_length]; // sentinel
  interprocess_semaphore lock; // write lock

  shm_buffer(): wi(0), ri(0), lock(1) {
    for(auto &cw: can_write) cw = true;
  }

  void write(int x) {
    lock.wait();     // lock to protect publishers from each other

    while (!can_write[wi]) {} // busy-spin
    msg[wi].data[0] = x;
    msg[wi].timestamp = high_resolution_clock::now();
    can_write[wi] = false;
    wi = (++wi) % buffer_length;

    lock.post();
  }

  message read() {
    while (can_write[ri]) {}  // busy-spin
    message m = msg[ri];
    can_write[ri] = true;
    ri = (++ri) % buffer_length;
    return m;
  }
};
