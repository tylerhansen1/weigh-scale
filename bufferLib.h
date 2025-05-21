/* 
 * File:   bufferLib.h
 * Author: alekshurson
 *
 * Created on April 2, 2024, 11:15 AM
 */

#ifndef BUFFERLIB_H
#define	BUFFERLIB_H

#ifdef	__cplusplus
extern "C" {
#endif

void putVal(float newValue);	// add a new value to the buffer
float getAvg();          	// average all buffer vals
void initBuffer();		// set all buffer vals to zero



#ifdef	__cplusplus
}
#endif

#endif	/* BUFFERLIB_H */

