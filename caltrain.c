#include "caltrain.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void print_station(struct station * station){

  printf("Passengers waiting : %d\nTrain Passengers: %d\n",station->passengersWaiting,station->trainPassengers );
}

void station_init(struct station *station){
  station->passengersWaiting=0;
  station->trainPassengers=0;
  station->enteredButNotBoarded=0;
  pthread_mutex_init(&(station->lock), NULL);
  pthread_cond_init(&(station->cond), NULL);
  pthread_cond_init(&(station->trainCond), NULL);
}

void station_load_train(struct station *station, int count){
  // printf("//////////////////////Load Train///////////////////////////////////////\n" );
  acquire(station , "Adding Train passengers");
  station->trainPassengers =count;
  station->enteredButNotBoarded = 0;
  // print_station(station);
  release(station , "Adding Train passengers");
  //Signal all threads.

  while(1){
      acquire(station , "Train loop");
      // print_station(station);
      pthread_cond_broadcast(&(station->cond));
      // printf("In Loop of loading train...\n");
      if(station->trainPassengers > 0 && station->passengersWaiting>0 ){
        //wait on condition.
        // printf("Waiting for passengers to enter ...\n" );
        pthread_cond_wait(&(station->trainCond), &(station->lock));
        // printf("Train Signaled\n" );
        release(station , "Train loop");
      }
      else if(station->enteredButNotBoarded !=0){
        // printf("Waiting for passengers to Boarding ...\n" );
        pthread_cond_wait(&(station->trainCond), &(station->lock));
        // printf("Train Signaled\n" );
        release(station , "Train loop");
      }
      else{
        release(station,"Train loop");
        break;
      }
    }

    acquire(station , "Making Train Passengers = 0");
    station->trainPassengers = 0;
    // print_station(station);
    release(station,"Making Train Passengers = 0");
    // printf("////////////////Train exited///////////////////////////\n");
}

void station_wait_for_train(struct station *station){
  acquire(station , 0);//"Increase waiting");
  station->passengersWaiting++;
  release(station , 0);//"Increase waiting");
  while(1){
    acquire(station , 0);//"Passenger checking for train");
    if(station->trainPassengers>0){
      // printf("Since remaining train passengers is %d\n",station->trainPassengers );
      station->passengersWaiting--;
      station->trainPassengers--;
      station->enteredButNotBoarded++;
      // station_on_board(station);//Before releasing, to avoid multiple boarding.
      release(station , 0);//"Passenger checking for train");
      break;
    }
    //Wait on train.
    pthread_cond_wait(&(station->cond), &(station->lock));

    //Notify
    release(station , 0);//"Passenger checking for train");
  }

}

void station_on_board(struct station *station){
  // printf("Passenger boarded! %d\n",station->trainPassengers);
  //Signal Train.
  acquire(station , 0);
  station->enteredButNotBoarded--;
  // printf("Boarded are %d.\n",station->enteredButNotBoarded);
  release(station , 0);
  pthread_cond_signal(&(station->trainCond));

}

void acquire(struct station *station , char* message){
  // if(message != 0)
  // printf("Acquired %s\n",message);

  pthread_mutex_lock(&(station->lock));
  // if(message !=0)
  //   printf("Critical %s\n",message);

}
void release(struct station *station , char* message){
  // if(message !=0)
  //   printf("Release %s\n",message);
  pthread_mutex_unlock(&(station->lock));
}
