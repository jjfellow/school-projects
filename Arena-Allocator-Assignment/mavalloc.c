/*
  Justin Fellows
  1001865403
*/

// The MIT License (MIT)
// 
// Copyright (c) 2021, 2022 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES UTA OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#include <stdlib.h>
#include <stdio.h>
#include "mavalloc.h"

//Want to say here, I wound up treating the list like the array that it is, and stopped using
//the next and previous members of the struct

//Defining the maximum number of entries in the linked list
#define MAXENTRIES 10000

//This is the enum that refers to whether the memory in the ledger entry is a Hole or a Process Allocation
enum TYPE
{
  H = 0,
  P = 1
};

//Defining the node structure, it has two void pointers that point to the start of its memory block for base,
//and the end of its block for limit. next and prev are indices that point to the next or previous
//node in the array, so entries do not have to be kept in sequential order
struct Node { 
  size_t size; 
  enum TYPE type; 
  void * arena; 
  int next; 
  int prev;
};

//Need the list to be accesible in all functions, as well as the algorithm
//so I'm declaring them as global variables
struct Node list[MAXENTRIES];
enum ALGORITHM alg;
//Won't know ahead of time which algorithm is being used, and next fit needs to remember where it left off
//So we need another global variable (don't smite me god)
int nextFitIndex;

int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
  //Checking that the size requested is greater than 0, and returning -1 if it isn't
  if(size <= 0)
  {
    return -1;
  }
  //First, make the requested amount of space a multiple of 4, so it is quicker to access
  //Since malloc returns a pointer to the requested number of bytes, making it a multiple
  //of 4 ensures that the memory can be evenly divided into 32-bit chunks
  size_t newSize = ALIGN4(size);
  list[0].arena = malloc(newSize);
  //If malloc() fails, it returns a null pointer. Check for that, and if it failed, return -1
  if(list[0].arena == NULL)
  {
    return -1;
  }
  //Keeping track of the size
  list[0].size = newSize;
  //This is the first node in the list, so it won't have a next node yet, or a previous node ever
  list[0].next = -1;
  list[0].prev = -1;
  list[0].type = H;
  //Finally, save the algorithm to be used and return a success
  alg = algorithm;
  if(algorithm == NEXT_FIT)
  {
    nextFitIndex = 0;
  }
  //Initialize each entry in the list to default values, is useful later
  int i;
  for(i = 1; i < MAXENTRIES; i++)
  {
    list[i].size = 0;
    list[i].type = H;
    list[i].next = -1;
    list[i].prev = -1;
    list[i].arena = NULL;
  }
  return 0;
}

void mavalloc_destroy( )
{
  //The bottom of the ledger should always point to the beginning of the managed memory
  //So to free the whole allocation, free the bottom's pointer
  free(list[0].arena);
  //Next, iterate over the entire list and set each entry to default values
  int i;
  for(i = 0; i < MAXENTRIES && list[i].size > 0; i++)
  {
    list[i].size = 0;
    list[i].next = -1;
    list[i].prev = -1;
    list[i].arena = NULL;
    list[i].type = H;
  }
  return;
}
//A helper function that shifts everything up, including the specified index
//The value of the specified index is not changed, but it can be safely overwritten
void shiftUp(int index)
{
  //do some quick input validation, if this evals to true then that's not our memory to access
  if(index >= MAXENTRIES)
  {
    return;
  }
  int i;
  //These are temporary Nodes that hold onto the value of the Node about to be overwritten
  struct Node shiftee = list[index];
  struct Node shifter;
  for(i = index + 1; i < MAXENTRIES && shiftee.size > 0; i++)
  {
    //Shift all the Nodes up
    //This is a triangle shaped operation, shifter grabs what is orignally in this spot in the list
    //Then this spot in the list is overwritten by what's in shiftee
    //Finally, shiftee is overwriteen to shifter's value
    //Tangentially related, this reminds me of dna polymerase
    shifter = list[i];
    list[i] = shiftee;
    shiftee = shifter;
  }
  //Anything on the end of the list "falls off", so there is potential for data loss
  //But this only happens if you try adding a Node to a full list

  for(i = 0; i < MAXENTRIES && list[i].size > 0; i++)
  {
    //Update each Node's prev and next members to reflect the shift
    if(list[i].next >= index)
    {
      list[i].next += 1;
    }

    if(list[i].prev >= index)
    {
      list[i].prev += 1;
    }
  }
  //Also, if we're using nextFit, need to adjust the nextFit index too
  if(alg == NEXT_FIT)
  {
    if(nextFitIndex >= index)
    {
      nextFitIndex += 1;
    }
  }
  return;

}


//Helper function to shift all entries past a supplied index down once
void shiftDown(int shiftdex)
{
  //Will need these variables inside the loop, shiftdex is the index pointing to an empty node
  int i;
  //Need to restrict this for loop from accessing the last node, since it will try to reach one more entry and go out of bounds of the array
  for(i = shiftdex; i < MAXENTRIES - 1 && list[i].size > 0; i++)
  {
    //Copying all values down along the list
    list[i] = list[i+1];
    
    //Next, if the next or previous node of that node points to a node that is greater than the shifting index, decrement them by one
    if(list[i].next > shiftdex)
    {
      list[i].next -= 1;
    }
    if(list[i].prev > shiftdex)
    {
      list[i].prev -= 1;
    }
  }

  //Since there can't be a valid values of the end of the list, must set end of the array to default values
  list[MAXENTRIES - 1].type =  H; 
  list[MAXENTRIES - 1].next = -1;
  list[MAXENTRIES - 1].prev = -1;
  list[MAXENTRIES - 1].arena = NULL;
  list[MAXENTRIES - 1].size =  0;

  //If we're using nextFit, then we'll also need to update the nextFitIndex
  if(alg == NEXT_FIT)
  {
    if(nextFitIndex >= shiftdex)
    {
      nextFitIndex -= 1;
    }
  }
  return;
}

//For readability, making each allocation algorithm its own function
void * nextAlloc( size_t size)
{
  //Jump through each entry, following the next and previous node, compare the size of the entry with requested size
  //If one is big enough(and a hole), update size in the node to requested size, and find an empty ledger entry to put remaining size
  //Important assumption: there will not be two holes next to each other, because they would have been merged when freed
  void * retPtr = NULL; //Return pointer variable, so we don't have to return immediately after allocating the memory
  //Need to keep the value of nextFitIndex intact, so copy its value to index
  int index = nextFitIndex;
  
  for(; list[index].size > 0 && index < MAXENTRIES; index++)
  {
    if(list[index].size >= size && list[index].type == H)
    {
      if(list[index].size == size)
      {
        //shorter version here, no shifting needed
        list[index].type = P;
        retPtr = list[index].arena;
        nextFitIndex = index + 1;
        return retPtr;

      }
      //If we've found a hole to fit our data in, shift the list up
      shiftUp(index);
      //Overwrite the extra entry at the index we've made
      list[index].next = index + 1;
      list[index].prev = list[index + 1].prev;
      list[index].size = size;
      list[index].type = P;
      list[index].arena = list[index + 1].arena;
      //Now fix the node we moved up
      list[index + 1].prev = index;
      list[index + 1].size = list[index + 1].size - size;
      list[index + 1].arena = (unsigned char *) list[index + 1].arena + size;
      //Assign and return the pointer we've just made
      retPtr = list[index].arena;
      nextFitIndex = index + 1;
      return retPtr;
    }
  }
  //If we leave the loop, then we didn't find a hole at nextFitIndex or later
  //So we need to loop back around
  //ShamelessCopyPaste.png

  for(index = 0; list[index].size > 0 && index < nextFitIndex; index++)
  {
    if(list[index].size >= size && list[index].type == H)
    {
      if(list[index].size == size)
      {
        //shorter version here, no shifting needed
        list[index].type = P;
        retPtr = list[index].arena;
        nextFitIndex = index + 1;
        return retPtr;

      }
      //If we've found a hole to fit our data in, shift the list up
      shiftUp(index);
      //Overwrite the extra entry at the index we've made
      list[index].next = index + 1;
      list[index].prev = list[index + 1].prev;
      list[index].size = size;
      list[index].type = P;
      list[index].arena = list[index + 1].arena;
      //Now fix the node we moved up
      list[index + 1].prev = index;
      list[index + 1].size = list[index + 1].size - size;
      list[index + 1].arena = (unsigned char *) list[index + 1].arena + size;
      //Assign and return the pointer we've just made
      retPtr = list[index].arena;
      nextFitIndex = index + 1;
      return retPtr;
    }
  }


  return retPtr;

}

void * firstAlloc( size_t size)
{
  //Can reuse next fit's algorithm because we don't need to be able to do multiple algorithms each instance
  //However, I think this means that first fit's data won't be much different from next fit in regards to runtime
  nextFitIndex = 0;
  return nextAlloc(size);
}
//Short helper function, finds the first hole entry in the list and returns its index
int getHole()
{
  int i = 0;
  for(;list[i].size > 0 && i < MAXENTRIES; i++)
  {
    if(list[i].type == H)
    {
      return i;
    }
  }
  //If no hole is found, return -1
  return -1;

}

//Look for the smallest possible hole to fit request in
void * bestAlloc(size_t size)
{
  //Keep track of current index, as well as the size and index of the smallest block possible to fit in
  int index = getHole();
  //Also keep track of the index of the smallest hole
  int smallestIndex = -1;
  //If getHole() returned -1, the arena has not been initialized or the arena is full, so return a null pointer
  if(index == -1)
  {
    return NULL;
  }
  
  //This is a bit hack-y, but if I initialize it to 0 it will never actually allocate anything
  size_t smallestSize = 999999999;
  //Because otherwise we'll potentially segfault here
  if(list[index].size >= size)
  {
    smallestIndex = index;
    smallestSize = list[index].size;
  }
  
  //Now, find the smallest hole that is at least the size requested
  for(;list[index].size > 0 && index < MAXENTRIES; index++)
  {
    //First, check if this listing is a hole
    if(list[index].type == H)
    {
      //If so, compare its size to the smallestSize and the requested size
      if(list[index].size < smallestSize && list[index].size >= size)
      {
        //If it meets both criteria, mark current index as the new smallest index and track its size
        smallestIndex = index;
        smallestSize = list[index].size;
      }
    }
  }
  //If smallestIndex is still -1, then no nodes have enough space to hold the requested memory
  //So return a null pointer
  if(smallestIndex == -1)
  {
    return NULL;
  }

  //Now that we have the smallest possible node this could fit in, time to allocate it and add an entry for any extra memory
  //dSize, short for delta Size, holds the difference in size between the requested size and the found size
  size_t dSize = smallestSize - size;
  //If dSize is 0, then it means that the block we're allocating is exactly the size requested, so only need to mark that block as
  //allocated before returning a pointer to its arena
  if(dSize == 0)
  {
    list[smallestIndex].type = P;
    return list[smallestIndex].arena;
  }
  //Shift the whole list up so we get room for our new entry
  //shiftUp does not delete or overwrite the node that's already at the supplied index
  //It copies it and everything after it up 1, so the current index is safe to overwrite
  shiftUp(smallestIndex);
  //Then link the new node into the list and update its size
  list[smallestIndex].size = size;
  list[smallestIndex].type = P;
  list[smallestIndex].next = smallestIndex + 1;
  list[smallestIndex + 1].prev = smallestIndex;
  list[smallestIndex + 1].size = dSize;
  list[smallestIndex + 1].arena = (unsigned char *) list[smallestIndex + 1].arena + size;
  
  //return the pointer the user requested
  return list[smallestIndex].arena;
}

//Look for the largest possible hole to fit request in
void * worstAlloc(size_t size)
{
  //Keep track of current index, as well as the size and index of the biggest block possible to fit in
  int index = getHole();
  //Also keep track of the index of the biggest hole
  int biggestIndex = -1;
  //If getHole() returned -1, the arena has not been initialized or the arena is full, so return a null pointer
  if(index == -1)
  {
    return NULL;
  }
  
  size_t biggestSize = 0;
  //Because otherwise we'll potentially segfault here
  if(list[index].size >= size)
  {
    biggestIndex = index;
  }

  //Now, find the biggest hole that is at least the size requested
  for(;list[index].size > 0 && index < MAXENTRIES; index++)
  {
    //First, check if this listing is a hole
    if(list[index].type == H)
    {
      //If so, compare its size to the biggestSize and the requested size
      if(list[index].size > biggestSize && list[index].size >= size)
      {
        //If it meets both criteria, mark current index as the new biggest index and track its size
        biggestIndex = index;
        biggestSize = list[index].size;
      }
    }
  }
  //If biggestIndex is still -1, then no nodes have enough space to hold the requested memory
  //So return a null pointer
  if(biggestIndex == -1)
  {
    return NULL;
  }
  //Now that we have the biggest possible node this could fit in, time to allocate it and add an entry for any extra memory
  //dSize, short for delta Size, holds the difference in size between the requested size and the found size
  size_t dSize = biggestSize - size;
  //If dSize is 0, then it means that the block we're allocating is exactly the size requested, so only need to mark that block as
  //allocated before returning a pointer to its arena
  if(dSize == 0)
  {
    list[biggestIndex].type = P;
    return list[biggestIndex].arena;
  }
  //Shift the whole list up so we get room for our new entry
  //shiftUp does not delete or overwrite the node that's already at the supplied index
  //It copies it and everything after it up 1, so the current index is safe to overwrite
  shiftUp(biggestIndex);
  //Then link the new node into the list and update its size
  list[biggestIndex].size = size;
  list[biggestIndex].type = P;
  list[biggestIndex].next = biggestIndex + 1;
  list[biggestIndex + 1].prev = biggestIndex;
  list[biggestIndex + 1].size = dSize;
  list[biggestIndex + 1].arena = (unsigned char *) list[biggestIndex + 1].arena + size;
  
  //return the pointer the user requested
  return list[biggestIndex].arena;
}

void * mavalloc_alloc( size_t size )
{
  //Don't forget to align the size request to a multiple of 4
  size_t reqSize = ALIGN4(size);
  //Declare the return pointer, it defaults to NULL
  void * retPtr = NULL;
  //Check which algorithm was decided in initialization and call that version of allocate
  if(alg == FIRST_FIT)
  {
    retPtr = firstAlloc(reqSize);
  }
  else if(alg == NEXT_FIT)
  {
    retPtr = nextAlloc(reqSize);
  }
  else if(alg == BEST_FIT)
  {
    retPtr = bestAlloc(reqSize);
  }
  else if(alg == WORST_FIT)
  {
    retPtr = worstAlloc(reqSize);
  }
  // return the return pointer; will only return NULL if something went wrong
  return retPtr;
}



//Helper function to find all holes that have a neighboring hole, and merge them
void mergeHoles()
{
  int i;
  for(i = 0; i < MAXENTRIES && list[i].size > 0; i++)
  {
    //Only need to merge entries if they are holes
    if(list[i].type == H && list[i+1].type == H)
    {
      //Only can merge if there is a next node to merge with
      if(list[i+1].size > 0)
      {
        //To merge, add the size of the next node to its own size, link the two new neighbor nodes,
        //and then shift down all other nodes
        list[i].size = list[i].size + list[i+1].size;
        list[i].next = list[i+1].next;
        //Here, we look two nodes ahead. If that node exists, make its previous attribute point to the merged node
        if(list[i+2].size > 0)
        {
          list[i+2].prev = i;
        }
        //After all that, compact the list
        shiftDown(i+1);
        //Also need to run the loop again on the entry we've just done, in case we moved another hole into the next node
        //when we shifted down. Subtracting 1 here will ensure the loop is run at the same index since the for loop adds 1 to i upon completion
        i -= 1;
      }
    }
  }

}

void mavalloc_free( void * ptr )
{
  //Validate the supplied pointer exists
  if(ptr == NULL)
  {
    return;
  }
  int i;
  for(i = 0; i < MAXENTRIES && list[i].size > 0; i++)
  {
    if(list[i].arena == ptr)
    {
      list[i].type = H;
      mergeHoles();
      return;
    }
  }

  return;
}

//Returns the number of nodes in the linked list
int mavalloc_size( )
{
  int number_of_nodes = 0;
  int i = 0;
  
  for(;list[i].size > 0 && i < MAXENTRIES; i++)
  {
    number_of_nodes += 1;
  }

  return number_of_nodes;
}
