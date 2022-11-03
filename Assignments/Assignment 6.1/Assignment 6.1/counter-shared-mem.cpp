#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/mman.h> 
#include <sys/stat.h>  /* For mode constants */ 
#include <fcntl.h> 
#include <sys/types.h> 
#include <errno.h> 
#include <semaphore.h> 
#include <stdint.h>

#define SHM_SEGMENT_SIZE 256 

#define SHM_SEGMENT_NAME "/demo-shm" 
#define SEMA_NAME "/demo-sem" 

static sem_t *demo_sem;
/* 
 * If the shared memory segment does not exist already, create it 
 * Returns a pointer to the segment or NULL if there is an error 
 */ 

static void *get_shared_memory(void) 
{ 
  int shm_fd; 
  void *shm_p; 
  /* Attempt to create the shared memory segment */ 

  shm_fd = shm_open(SHM_SEGMENT_NAME, O_CREAT | O_EXCL | O_RDWR, 0666); 
 
  if (shm_fd > 0) { 
    /* succeeded: expand it to the desired size (Note: dont't do 
     "this every time because ftruncate fills it with zeros) */ 
    printf ("Creating shared memory and setting size=%d\n", SHM_SEGMENT_SIZE); 

    if (ftruncate(shm_fd, SHM_SEGMENT_SIZE) < 0) { 
      perror("ftruncate"); 
      exit(1); 
    } 

    /* Create a semaphore as well */ 
    demo_sem = sem_open(SEMA_NAME, O_RDWR | O_CREAT, 0666, 1); 
    if (demo_sem == SEM_FAILED) 
      perror("sem_open failed\n"); 
  } 

  else if (shm_fd == -1 && errno == EEXIST) { 
    /* Already exists: open again without O_CREAT */ 
    shm_fd = shm_open(SHM_SEGMENT_NAME, O_RDWR, 0); 
    demo_sem = sem_open(SEMA_NAME, O_RDWR); 

    if (demo_sem == SEM_FAILED) 
      perror("sem_open failed\n"); 
  } 

  if (shm_fd == -1) { 
    perror("shm_open " SHM_SEGMENT_NAME); 
    exit(1); 
  } 

  /* Map the shared memory */ 

  shm_p = mmap(NULL, SHM_SEGMENT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); 

  if (shm_p == NULL) { 
    perror("mmap"); 
    exit(1); 
  } 

  return shm_p; 
}

int main(int argc, char *argv[]) 

{ 
  printf("%s PID=%d\n", argv[0], getpid()); 
  uint8_t* shm_p = (uint8_t*) get_shared_memory(); 
  *shm_p = (uint8_t) 0;

  while (1) { 
    usleep(1000000); 
    sem_wait(demo_sem); 
    
    *shm_p = *shm_p + 1;
    printf("%d\n", *shm_p); 

    /* Write our signature to the shared memory */ 
    sem_post(demo_sem); 
  } 
  return 0; 
} 