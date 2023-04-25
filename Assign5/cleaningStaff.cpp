#include "main.hpp"

void *cleaningStaff(void *arg){

    int cid = *(int *)arg;


    while(1){

        sem_wait(&cleaner_sem);

        // cleaning staff will clean first uncleaned rooom
        if(!doneRooms.empty()){
            Room *room = doneRooms.front();
            printf("Cleaning staff %d will clean till %d seconds\n", cid, room->time_used);
            doneRooms.pop();
            // cleaning staff will clean the room
            sem_post(&cleaner_sem);
            sleep(room->time_used);
        }
        else{
            sem_post(&room_sem);
        }
    }
}