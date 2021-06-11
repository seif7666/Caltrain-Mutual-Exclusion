
#include <pthread.h>

struct station {
  int passengersWaiting;
  int trainPassengers;
  pthread_mutex_t lock; //Lock to ensure no race condition occurs.
  pthread_cond_t cond;
  pthread_cond_t trainCond;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);

void acquire(struct station *station , char*);
void release(struct station *station ,char*);
