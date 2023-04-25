#include "main.hpp"

int guests_entered = 0;
int x, y, n;
queue<Room *> freeRooms;
set<pair<int, Room *>> occupiedRooms;
queue<Room *> doneRooms;

sem_t cleaner_sem, room_sem;
pthread_t *guests, *cleaningStaffs;

int main()
{
    srand(time(NULL));
    cout << "Enter x, y, n: ";
    cin >> x >> y >> n;

    guests = new pthread_t[y];
    cleaningStaffs = new pthread_t[x];

    vector<int> numbers;
    for (int i = 1; i <= y; i++)
    {
        numbers.push_back(i);
    }
    std::random_shuffle(numbers.begin(), numbers.end());

    // init semaphores
    sem_init(&room_sem, 0, 1);
    sem_init(&cleaner_sem, 0, 0);
    for (int i = 0; i < n; ++i)
    {
        Room *room = new Room();
        freeRooms.push(room);
    }

    for (int i = 0; i < y; ++i)
    {
        pthread_create(&guests[i], NULL, guest, (void *)&numbers[i]);
    }

    for (int i = 0; i < x; ++i)
    {
        pthread_create(&cleaningStaffs[i], NULL, cleaningStaff, (void *)&i);
    }

    for (int i = 0; i < y; ++i)
    {
        pthread_join(guests[i], NULL);
    }

    for (int i = 0; i < x; ++i)
    {
        pthread_join(cleaningStaffs[i], NULL);
    }

    // destroy semaphores
    sem_destroy(&room_sem);
    sem_destroy(&cleaner_sem);
}
