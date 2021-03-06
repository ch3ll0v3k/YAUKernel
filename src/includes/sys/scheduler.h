#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <sys/interrupts.h>
#include <sys/task.h>

void scheduler_add_task(struct task *t);
void schedule(struct pushed_registers *regs);
void scheduler_switch_to_task(struct task *t);

#endif