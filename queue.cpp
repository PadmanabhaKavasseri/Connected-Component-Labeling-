//Author:Padmanabha Kavasseri

#include "queue.h"

Queue::Queue(int maxlen) {
    contents = new Location[maxlen];
    head = 0;
    tail = 0;
}


Queue::~Queue() {
    delete[] contents;
}

// insert a new Location at the end/back of the list
void Queue::push(Location loc) {
    contents[tail]=loc;
    tail++;
}

// return and "remove" the oldest Location not already extracted
Location Queue::pop() {
//remove from front
    head++;
    return contents[head-1];
}

//checks if the queue is empty
bool Queue::is_empty() {
    return (head==tail);
}