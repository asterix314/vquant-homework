/***********************
   buffer.hpp 
   messages and buffer operations, macro constants.
   for homework project

   zhang chen hui
   asterix314@163.com

1. 把微秒计时改为纳秒计时；
2. 测试时使用pthread_setaffinity_np或者taskset将publisher和subscriber锁到不同核上；
3. 避免publisher和subscriber上的线程切换。我这里提供两个提示：1） publisher和subscriber在常态下应该是在busy spin; 2）. 使用无锁的方式

use sched_setaffinity for processes.
better: taskset or cpuset

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

constexpr int message_size = 256;        // message size in bytes.
constexpr int num_messages = 1000;       // #messages to send per publisher
constexpr int num_publishers = 2;        // #publishers
constexpr int num_subscribers = 1;       // #subscribers
constexpr int buffer_length = 16;        // #messages to hold in buffer

using namespace std::chrono;

struct message {
  high_resolution_clock::time_point timestamp;
  int data[(message_size - sizeof(timestamp)) / sizeof(int)];
};

struct check_record {   // for statistics
  double elapsed_ns;    // nanoseconds elapsed
  int data;
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

  check_record pop() {
    m_full.wait();
    m_mutex.wait();

    std::chrono::duration<double, std::nano> elapsed = 
      high_resolution_clock::now() - msg[ri].timestamp;
    check_record check = {
      elapsed.count(),
      msg[ri].data[0]
    };
    ri = (++ri) % buffer_length;

    m_mutex.post();
    m_empty.post();

    return check;
  }

};
