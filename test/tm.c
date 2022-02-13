#include "tm.h"

typedef struct {
  uint8_t id;
  uint8_t prio;
  uint8_t attached;
  uint8_t *func;
  char name[MAX_NAME_LEN];
} task;

static task curTask;
static uint8_t numOfTasks = 0;
static task taskList[MAX_TASKS];

uint8_t TM_attach(char *name, uint8_t prio, void *fn) {
  uint8_t result = 0;
  if (strlen(name) > MAX_NAME_LEN) return result;
  if (numOfTasks > MAX_TASKS) return result;
  taskList[numOfTasks].id = numOfTasks;
  strcpy(taskList[numOfTasks].name, name);
  taskList[numOfTasks].prio = prio;
  taskList[numOfTasks].func = fn;
  taskList[numOfTasks].attached = 1;
  result = 1;
  numOfTasks++;
  return result;
}

uint8_t TM_setPrio(uint8_t id, uint8_t prio) {
  if (id < 0 || id > MAX_TASKS) return 0;
  taskList[id].prio = prio;
  return 1;
}

uint8_t TM_detach(uint8_t id) {
  if (id < 0 || id > MAX_TASKS) return 0;
  taskList[id].attached = 0;
  return 1;
}

int8_t TM_getId(char *name) {
  for (uint8_t i = 0; i < MAX_TASKS; i++) {
    if (strcmp(taskList[i].name, name)) return i;
  } return -1;
}

uint8_t TM_getPrio(uint8_t id) {
  if (id < 0 || id > MAX_TASKS) return 0;
  return taskList[id].prio;
}

uint8_t TM_getCurTask(void) {
  return curTask.id;
}

void TM_start(void) {
  uint8_t taskNum = 0;
  void (*fn[MAX_TASKS])(void);
  uint8_t prioArr[MAX_TASKS] = {0};
  uint8_t reserve[MAX_TASKS] = {0};
  for (uint8_t i = 0; i < MAX_TASKS; i++) {
    if (taskList[i].attached) {
      fn[taskNum++] = (void *)taskList[i].func;
      prioArr[taskNum] = taskList[i].prio;
    }
  }
  for (uint8_t i = 0; i < taskNum; i++) {
    uint8_t maxPrio = 0;
    uint8_t needIdx = 0;
    for (uint8_t j = 0; j < taskNum; j++) {
      if (prioArr[j] > maxPrio) {
        maxPrio = prioArr[j];
        needIdx = j;
      }
    }
    printf("%d %d\n", maxPrio, needIdx);
    fn[needIdx]();
    reserve[needIdx] = prioArr[needIdx];
    prioArr[needIdx] = 0;
    curTask = taskList[needIdx];
  }
  for (uint8_t i = 0; i < taskNum; i++) {
    prioArr[i] = reserve[i];
    reserve[i] = 0;
  }
}
