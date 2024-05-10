#ifndef RUPPVM_H
#define RUPPVM_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


typedef struct VM_Operand VM_Operand;
typedef struct VM_Stack VM_Stack;
typedef struct VM_Thread VM_Thread;
typedef struct VM_Sched VM_Sched;
typedef struct VM_Memory VM_Memory;
typedef struct VM_State VM_State;

typedef VM_Operand
{
  union
  {
    uintptr_t mem_value;
    intmax_t const_value;
    ptrdiff_t addr_value;
  };

  enum
  {
    OP_MEM,
    OP_CONST,
    OP_ADDR,
  }
  kind;

  bool is_alive;
  bool is_thread_local;
  
  VM_Operand *next;
};

struct VM_Stack
{
   VM_Operand *root;
   VM_Operand *head;
   VM_Operand *next;
   VM_Opernad *prev;
};


struct VM_Thread
{
   ssize_t thread_id;
   VM_Operand *entry_point;
   VM_Opernad *entry_args;

   enum
   {
     TH_RUNNING,
     TH_READY,
     TH_ZOMBIE,
     TH_DONE,
   }
   state;

   VM_Thread *next;
};

struct VM_Sched
{
  VM_Thread *head;
  VM_Thread *current;
  size_t num_threads;

  time_t last_sync;
};

struct VM_Memory
{
   ptrdiff_t mem_start;
   ptrdiff_t mem_end;

   time_t last_collect;
};

struct VM_State
{
   VM_Sched *sched;
   VM_Memory *memory;
   VM_Stack *stack;
};


#endif
