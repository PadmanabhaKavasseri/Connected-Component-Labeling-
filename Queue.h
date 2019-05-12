
//Description: Declares two new data types,
//- Queue, a list of Locations organized chronologically
#ifndef QUEUE_H
#define QUEUE_H

struct Location {
   int row;
   int col;
};

// a class that contains a series of Locations
class Queue {
   public: 
   
   Queue(int maxlen);
   
   ~Queue();
   
   // insert a new Location at the end/back of the list   
   void push(Location loc);

   // return and "remove" the oldest Location not already extracted
   Location pop();

   // is this Queue empty? (did we extract everything added?)
   bool is_empty();

   // member variables of an Queue, for implementation:
   private:
   Location* contents; // ptr to dynamically-allocated array
   int tail; // Index of the next free item at the end.

   int head; // Index of the first occupied item    
};

#endif