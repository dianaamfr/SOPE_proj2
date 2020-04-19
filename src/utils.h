/**
 * @file utils.h
 * @author SOPE Group T02G05
 * @brief File containing utilities' headers and constants
 */

#ifndef UTILS_H
#define UTILS_H

#include "types.h"

/**
 * @brief Print args captured with @see checkArgs()
 * @param a args to be printed
 * @note For Debugging Purposes
*/
void printArgs(args * a);

/** 
 * @brief Fills the @p args, by checking the arguments passed to the function
 * @param argc number of args passed
 * @param argv args passed
 * @param a struct to be filled with args
 * @param C caller program
 * @return OK if successful, ERROR otherwise
 * @note Lots of Credits to those who managed to give a very good explanation at 
 * @see https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
 * @see https://linux.die.net/man/3/getopt_long for more info on the functions used
 */
int checkArgs(int argc, char * argv[], args * a, caller C);

/**
 * @brief Creates a Fifo with name passed in @p fifoName
 * @param fifoName the name of the named Fifo
 * @return OK if successful, ERROR otherwise
*/
int createFifo(char * fifoName);

/**
 * @brief Destroy a Fifo with name passed in @p fifoName
 * @param fifoName the name of the named Fifo
 * @return OK if successful, ERROR otherwise
*/
int killFifo(char * fifoName);

#endif
