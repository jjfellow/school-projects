/*
    Justin Fellows
    1001865403
*/

#include "include/crypto.h"
#include "schedule.h"
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "clock.h"

#define MAX_NUM_THREADS 1024 
#define MAX_FILENAME_LENGTH 255
#define BUFFER_SIZE 148

static char * message_buffer[BUFFER_SIZE];
static int count ;
static int running = 1 ;

pthread_t message_receiver_tid ;
pthread_t decryptor_tid[ MAX_NUM_THREADS ] ;
//Since only one thread is allowed the mutex lock at a time, I'm calling it cookieJar
//GET YOUR HANDS OFF MY COOKIES
pthread_mutex_t cookieJar;
//And Java puns are never not funny to me

static void handleSIGUSR2( int sig )
{
  printf("Time to shutdown\n");
  running = 0 ;
}

int insertMessage( char * message )
{
  assert( count < BUFFER_SIZE && "Tried to add a message to a full buffer");
  strncpy( message_buffer[count] , message, MAX_FILENAME_LENGTH ); 
  count++;
  
  return 0;
}

int removeMessage( char *message )
{
  assert( count && "Tried to remove a message from an empty buffer");
  strncpy( message, message_buffer[count-1], MAX_FILENAME_LENGTH ); 
  count--;

  return 0;
}

static void * tick ( void ) 
{
   return NULL ;
}

void * receiver_thread( void * args )
{
  
  while( running )
  {
    
    char * message_file = retrieveReceivedMessages( );
    //We only need to lock the critical region when we are writing to it
    if( message_file )
    {
      pthread_mutex_lock(&cookieJar);
      insertMessage( message_file ) ;
      pthread_mutex_unlock(&cookieJar);
    }
    
  }
  
}

void * decryptor_thread( void * args )
{
  while( running )
  {
    char * input_filename  = ( char * ) malloc ( sizeof( char ) * MAX_FILENAME_LENGTH ) ;
    char * output_filename  = ( char * ) malloc ( sizeof( char ) * MAX_FILENAME_LENGTH ) ;
    char * message = ( char * ) malloc ( sizeof( char ) * MAX_FILENAME_LENGTH ) ;

    memset( message,         0, MAX_FILENAME_LENGTH ) ;
    memset( input_filename,  0, MAX_FILENAME_LENGTH ) ;
    memset( output_filename, 0, MAX_FILENAME_LENGTH ) ;
    //Only need to lock the critical region when we are reading from it and writing to it
    //So each thread can do its allocation and deallocation in parrallel
    //Need to lock before checking count in case count changes after we check it but before we lock
    pthread_mutex_lock(&cookieJar);
    if(count != 0)
    {
      removeMessage( message );

      strncpy( input_filename, "ciphertext/", strlen( "ciphertext/" ) ) ;
      strcat ( input_filename, message );

      strncpy( output_filename, "results/", strlen( "results/" ) ) ;
      strcat ( output_filename, message );
      output_filename[ strlen( output_filename ) - 8 ] = '\0';
      strcat ( output_filename, ".txt" );

      decryptFile( input_filename, output_filename );
    }
    pthread_mutex_unlock(&cookieJar);

    free( input_filename ) ;
    free( output_filename ) ;
    free( message ) ;
  }
}

int main( int argc, char * argv[] )
{
    if( argc != 2 )
    {
      printf("Usage: ./a.out [number of threads]\n") ;
    }
    int num_threads = atoi( argv[1] ) ;
    pthread_t tid[ MAX_NUM_THREADS ] ;

    // initialize the message buffer
    int i ;
    for( i = 0; i < BUFFER_SIZE; i++ )
    {
        message_buffer[i] = ( char * ) malloc( MAX_FILENAME_LENGTH ) ;
    }

    count = 0 ;

    struct sigaction act;
    memset ( & act, '\0', sizeof( act ) ) ;
    act . sa_handler = & handleSIGUSR2 ;

    if ( sigaction( SIGUSR2, &act, NULL ) < 0 )  
    {
      perror ( "sigaction: " ) ;
      return 0;
    }

    initializeClock( ONE_SECOND ) ;
    registerWithClock( tick ) ;

    initializeSchedule( "schedule.txt" ) ;
    //NOTE: When creating a thread, you pass in a function that each thread does before terminating
    //So we've created the mutex as a global variable(boo hiss) and initialize it here
    pthread_mutex_init(&cookieJar, NULL);

    pthread_create( &message_receiver_tid, NULL, receiver_thread, NULL ) ;

    for( i = 0; i < num_threads; i++ )
    {
      pthread_create( & decryptor_tid[i], NULL, decryptor_thread, NULL ) ;
    }

    startClock( ) ;

    while( running ) ;

    stopClock( ) ;

    pthread_join( message_receiver_tid, NULL ) ;

    for( i = 0; i < num_threads; i++ )
    {
      pthread_join( decryptor_tid[i], NULL ) ;
    }

    for( i = 0; i < BUFFER_SIZE; i++ )
    {
        free( message_buffer[i] ) ;
    }
    freeSchedule( ) ;

    return 0 ;
}
