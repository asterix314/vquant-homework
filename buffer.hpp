#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <chrono>

#define MESSAGE_SIZE 256        // message size in bytes.
#define NUM_MESSAGES 1000       // #messages to send per publisher
#define NUM_PUBLISHERS 2        // #publishers
#define NUM_SUBSCRIBERS 1       // #subscribers
#define BUFFER_LENGTH 16        // #messages to hold in buffer

using namespace std::chrono;

struct message {
  high_resolution_clock::time_point timestamp;
  int data[MESSAGE_SIZE / sizeof(int)];
};

struct check_record {   // for statistics
  double elapsed_us;    // microseconds elapsed
  int data;
};

struct shm_buffer {     // shared memory buffer 
  message messages[BUFFER_LENGTH];
  int write_index = 0;
  int read_index = 0;

  boost::interprocess::interprocess_semaphore
    m_mutex,  // buffer mutex
    m_empty,  // semaphore on empty slots
    m_full;   // semaphore on full slots

  shm_buffer():
    m_mutex(1),
    m_empty(BUFFER_LENGTH),
    m_full(0)
  {}

  void push(int x) {
    m_empty.wait();
    m_mutex.wait();
    
    messages[write_index] = {
      high_resolution_clock::now(), {x}
    };
    write_index = (++write_index) % BUFFER_LENGTH;

    m_mutex.post();
    m_full.post();
  }

  check_record pop() {
    m_full.wait();
    m_mutex.wait();

    std::chrono::duration<double, std::micro> elapsed = 
      high_resolution_clock::now() - messages[read_index].timestamp;
    check_record check = {
      elapsed.count(),
      messages[read_index].data[0]
    };
    read_index = (++read_index) % BUFFER_LENGTH;

    m_mutex.post();
    m_empty.post();

    return check;
  }

};
