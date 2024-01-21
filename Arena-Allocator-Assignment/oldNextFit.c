


  //Ignore this mess down here vvvv, we can do better
  for(;index > MAXENTRIES; index++)
  {
    //printf("Current Index is %d\n", index);
    //If we find a Hole that is at least as big as the size needed
    if(list[index].type == H && list[index].size >= size)
    {
      //Debug print statements
      printf("Index is %d\n", index);
      //Mark the index as a Program Allocation
      list[index].type = P;
      //Set the change in size, so we know how big to make the next entry
      printf("Size in node 0 is %lu and requested size is %lu\n", list[index].size, size);
      dSize = list[index].size - size;
      //Grab the pointer to the allocated memory we are going to return
      retPtr = list[index].arena;
      printf("Retprt is %p", retPtr);
      //Check if the change in size is 0, which would be when the hole is exactly the same size as the requested memory
      if(dSize == 0)
      {
        //If so, point nextFitIndex at either 0 or the next index in the chain
        if(list[index].next == -1)
        {
          nextFitIndex = 0;
        }
        else
        {
          nextFitIndex = list[index].next;
        }

        return retPtr;
      }
      list[index].size = size;
      int newNext = 0;
      //Finding the end of the list to place the new entry
      for(;(list[newNext].next != -1 || list[newNext].prev != -1) && newNext < MAXENTRIES; newNext +=1)
      {/*Using an empty for loop to find the end of the list*/}
      //Setting the size of the new entry, it uses the change in Size calculated earlier
      list[newNext].size = dSize;
      //Linking the two nodes
      list[newNext].prev = index;
      list[index].next = newNext;
      //Pointer arithmetic cannot be performed on void pointers, so we cast the pointer in the node entry to unsigned char
      //This is because sizeof(unsigned char) is 1 byte, which means the addition is 1 byte for 1 byte
      list[newNext].arena = (unsigned char *) list[index].arena + dSize;
      //Then return the running pointer to the requested memory after doing our bookkeeping
      return retPtr;

    }
  }
  //If nothing was found after the nextFitIndex, then we circle back around
  index = 0;
  //more copy paste, I am going to get smote for this
  while(index != nextFitIndex)
  {
    //If we find a Hole that is at least as big as the size needed
    if(list[index].type == H && list[index].size >= size)
    {
      //Mark the index as a Program Allocation
      list[index].type = P;
      //Set the change in size, so we know how big to make the next entry
      dSize = list[index].size - size;
      //Grab the pointer to the allocated memory we are going to return
      retPtr = list[index].arena;
      //Check if the change in size is 0, which would be when the hole is exactly the same size as the requested memory
      if(dSize == 0)
      {
        //If so, point nextFitIndex at either 0 or the next index in the chain
        if(list[index].next == -1)
        {
          nextFitIndex = 0;
        }
        else
        {
          nextFitIndex = list[index].next;
        }

        return retPtr;
      }
      list[index].size = size;
      int newNext = list[index].next;
      //Finding the end of the list to place the new entry
      for(;list[newNext].next > -1 && newNext < MAXENTRIES; newNext++){/*Using an empty for loop to find the end of the list*/}
      //The for loop gets the index to the last entry in the ledger, need to go one more to get to the empty next entry
      newNext += 1;
      //Setting the size of the new entry, it uses the change in Size calculated earlier
      list[newNext].size = dSize;
      //Linking the two nodes
      list[newNext].prev = index;
      list[index].next = newNext;
      //Pointer arithmetic cannot be performed on void pointers, so we cast the pointer in the node entry to unsigned char
      //This is because sizeof(unsigned char) is 1 byte, which means the addition is 1 byte for 1 byte
      list[newNext].arena = (unsigned char *) list[index].arena + dSize;
      //Then return the running pointer to the requested memory after doing our bookkeeping
      return retPtr;
    }

    index = list[index].next;
  }
  //If, after all this searching, no acceptable memory location is found, return a null pointer
  
  return retPtr; //What a mess, refactor this whenever possible