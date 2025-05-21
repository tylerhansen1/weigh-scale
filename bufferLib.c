#include "xc.h"
#include "bufferLib.h"

#define SIZE 10
#define NUMSAMPLES 10
	
float buffer[SIZE];
int pointer = 0;

void initBuffer() {
// set all buffer vals to zero
    
    for (int i = 0; i < SIZE; i++) {
        // sets all values to 0
        buffer[i] = 0;
    }
    
    pointer = 0; // resets index
    return;
}		


void putVal(float newValue) {
// add a new value to the buffer
    
    buffer[pointer++] = newValue; // sets current buffer index to new value and increments index value
    
    if (pointer >= SIZE) { // resets index if out of bounds
        pointer = 0;
    }
    
    return;
}


float getAvg() {	
// average all buffer vals
    
    int total = 0;
//    int startPointer = pointer;
    for (int i = 0; i < SIZE; i++) { 
        // adds every element in buffer to 'total'
        total += buffer[i];
//        startPointer = (startPointer + 1) % SIZE;
    }
    return total / SIZE;
}
