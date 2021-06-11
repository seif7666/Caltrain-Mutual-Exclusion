#include "caltrain.h"
#include <stdio.h>

int main(){
  struct station station;
  station_init(&station);
  return 0;
}

void station_init(struct station *station){
  station->passengersWaiting=0;
  station->trainPassengers=0;
}

void station_load_train(struct station *station, int count){
  pthread_mutex_lock((&station->lock));
  station->trainPassengers =count;
  pthread_mutex_unlock(&(station->lock));

  //Signal all threads.
  pthread_cond_broadcast(&(station->cond));


  while(1){
      pthread_mutex_lock((&station->lock));
      if(station->trainPassengers > 0 && station->passengersWaiting>0){
        //wait on condition.
        pthread_cond_wait(&(station->trainCond), &(station->lock));
        pthread_mutex_unlock(&(station->lock));
      }
      else{
        pthread_mutex_unlock(&(station->lock));
        break;
      }

    }
}

void station_wait_for_train(struct station *station){
  pthread_mutex_lock((&station->lock));
  station->passengersWaiting++;
  pthread_mutex_unlock(&(station->lock));
  while(1){
    pthread_mutex_lock((&station->lock));
    if(station->trainPassengers>0){
      station_on_board(station);//Before releasing, to avoid multiple boarding.
      pthread_mutex_unlock(&(station->lock));
      break;
    }
    //Wait on train.
    pthread_cond_wait(&(station->cond), &(station->lock));
    pthread_mutex_unlock(&(station->lock));
    //Notify
  }
}

void station_on_board(struct station *station){
  station->passengersWaiting--;
  station->trainPassengers--;
  //Signal Train.
  pthread_cond_signal(&(station->trainCond));
}
