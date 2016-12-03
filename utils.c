/*
 * utils.c
 *
 *  Created on: Dec 3, 2016
 *      Author: nirattar
 */
#include "utils.h"

//will get a char pointer and its length.
//will zero out all the chars up to length.
void ZeroCharArr(char [] arr, int length)
{
	for (int i=0; i<length; i++)
	{
		arr[i] = 0;
	}
}
