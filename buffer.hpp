/***********************
   buffer.hpp 
   messages and buffer operations, macro constants.
   for homework project

   zhang chen hui
   asterix314@163.com
***********************/

#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <chrono>

constexpr int message_size = 256;        // message size in bytes.
constexpr int num_messages = 2000;       // #messages to send per publisher
constexpr int num_publishers = 2;        // #publishers
constexpr int num_subscribers = 1;       // #subscribers
constexpr int buffer_length = 16;        // #messages to hold in buffer

using namespace std::chrono;

struct message {
  high_resolution_clock::time_point timestamp;
  int data[(message_size - sizeof(timestamp)) / sizeof(int)];
};

struct shm_buffer {     // shared memory buffer 
  message msg[buffer_length];
  int wi = 0;    // wirte index
  int ri = 0;    // read index

  boost::interprocess::interprocess_semaphore
    m_mutex,  // buffer mutex
    m_empty,  // semaphore on empty slots
    m_full;   // semaphore on full slots

  shm_buffer():
    m_mutex(1),
    m_empty(buffer_length),
    m_full(0)
  {}

  void push(int x) {
    m_empty.wait();
    m_mutex.wait();
    
    msg[wi] = {
      high_resolution_clock::now(), {x}
    };
    wi = (++wi) % buffer_length;

    m_mutex.post();
    m_full.post();
  }

  message pop() {
    m_full.wait();
    m_mutex.wait();
 
    message m = msg[ri];
    ri = (++ri) % buffer_length;

    m_mutex.post();
    m_empty.post();

    return m;
  }

};
