#include "tm.h"
#include "tm.c"

#include <unistd.h>

static int prio = 38;
static uint8_t ready = 0;

void taskInit(void) {
  printf("Void task");
}

void task0(void) {
  printf("Task 0 started\n");
  printf("Waiting 2 seconds...\n");
  sleep(2);
  printf("2 seconds passed, voiding task 0...\n");
}

void task1(void) {
  printf("Task 1 started\n");
  printf("Waiting 1 second...\n");
  sleep(1);
  printf("1 second passwd, voiding task 1\n");
}

void task2(void) {
  printf("Task 2 started\n");
  printf("Changing self priority\n");
  sleep(1);
  prio++;
  printf("Priority: %d\n", prio);
  ready = !ready;
}

int main(void) {
  TM_attach("init", 255, taskInit);
  TM_attach("task0", 45, task0);
  TM_attach("task1", 40, task1);
  TM_attach("task2", prio, task2);
  while (1) {
    TM_setPrio(TM_getId("task2"), prio);
    if (ready) TM_setPrio(TM_getId("task0"), 255);
    else TM_setPrio(TM_getId("task0"), 1);
    TM_start();
    printf("\n\n");
  }
}
