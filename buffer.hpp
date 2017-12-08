/***********************
   buffer.hpp 
   messages and buffer operations, macro constants.
   for homework project

   zhang chen hui
   asterix314@163.com

pop: busy-spin
	repeat 2n:
		while (msg[ri].status != written);
		read from msg[ri];
		msg[ri].status = read;
		ri++;

push:
	repeat n:
		lock(mutex)
		while (msg[wi].status != read);
		write to msg[wi];
		msg[wi].status = written;
		wi++;
		unlock(mutex);
		sleep(1ms);

***********************/

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <chrono>

using namespace std::chrono;
using namespace boost::interprocess;

constexpr int message_size = 256;        // message size in bytes.
constexpr int num_messages = 3000;       // #messages to send per publisher
constexpr int num_publishers = 2;        // #publishers
constexpr int num_subscribers = 1;       // #subscribers
constexpr int buffer_length = 16;        // #messages to hold in buffer

struct message {
  enum sync_status {
    can_read,
    can_write
  } status;
  high_resolution_clock::time_point timestamp;
  int data[message_size / sizeof(int)];
};

// shared memory buffer 
struct shm_buffer {
  int wi = 0;           // wirte index
  int ri = 0;           // read index
  message msg[buffer_length];
  interprocess_semaphore lock; // write lock

  shm_buffer(): wi(0), ri(0), lock(1) {
    for(auto &m: msg) m.status = message::can_write;
  }

  void push(int x) {
    lock.wait();     // lock to protect publishers from each other

    while (msg[wi].status != message::can_write) {} // busy-spin
    msg[wi].data[0] = x;
    msg[wi].timestamp = high_resolution_clock::now();
    msg[wi].status = message::can_read;
    wi = (++wi) % buffer_length;

    lock.post();
  }

  message pop() {
    while (msg[ri].status != message::can_read) {}  // busy-spin
    message m = msg[ri];
    msg[ri].status = message::can_write;
    ri = (++ri) % buffer_length;
    
    return m;
  }

};
