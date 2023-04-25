#ifndef __MAIN_HPP
#define __MAIN_HPP

#include <iostream>
#include <pthread.h>
#include <set>
#include <utility>
#include <semaphore.h>
#include <unistd.h>
#include <queue>
#include <time.h>
#include <signal.h>
#include <vector>
#include <algorithm>
using namespace std;

extern int guests_entered;
extern sem_t cleaner_sem, room_sem;
extern pthread_t *guests, *cleaningStaffs;

extern int x, y, n;

class Room
{
public:
    int guests_used; // number of guests used
    int time_used; // total time used
    int priority; // priority of the guest staying in the room
    pthread_t guest_thread; // thread id of the guest staying in the room
    Room(int priority = -1, int guests_used = 0, int time_used = 0) : guests_used(guests_used), time_used(time_used), priority(priority) {}
};

extern void *guest(void *arg);
extern void *cleaningStaff(void *arg);

extern queue<Room *> freeRooms;
extern set<pair<int, Room *>> occupiedRooms;
extern queue<Room *> doneRooms;

#endif