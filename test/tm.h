#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_TASKS 8
#define MAX_NAME_LEN 16

typedef struct {
  uint8_t id;
  uint8_t prio;
  char* name[MAX_NAME_LEN];
  uint8_t *fn;
  uint8_t attached;
} task;

class TaskManager {
public:
  void init(void);
  void start(void);
  void stop(void);
  uint8_t attach(char *name, uint8_t prio, void *fn);
  uint8_t detach(uint8_t id);
  uint8_t setPrio(uint8_t id, uint8_t prio);
  uint8_t getId(char *name);
  uint8_t getPrio(uint8_t id);
  uint8_t getCurTask(void);
private:
  uint8_t maxTasks;
  task curTask;
  task taskList[MAX_TASKS];
};

#endif
