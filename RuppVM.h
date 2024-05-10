#ifndef RUPPVM_H
#define RUPPVM_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

typedef uintmax_t vm_ident_t;

typedef struct VM_Operand 	VM_Operand;
typedef struct VM_Stack 	VM_Stack;
typedef struct VM_Frame		VM_Frame;
typedef struct VM_Thread 	VM_Thread;
typedef struct VM_Sched 	VM_Sched;
typedef struct VM_Arena 	VM_Arena;
typedef struct VM_Memory 	VM_Memory;
typedef struct VM_State 	VM_State;

struct VM_Operand
{
  union
  {
    uintptr_t 	mem_value;
    intmax_t 	const_value;
    ptrdiff_t 	addr_value;
  };

  enum
  {
    OP_MEM,
    OP_CONST,
    OP_ADDR,
  }
  kind;

  vm_ident_t operand_id;

  bool is_alive;
  bool is_static;
  bool is_thread_local;
  bool is_opcode;
  
  VM_Operand *next;
};

struct VM_Frame
{
   VM_Operand *root_operand;
   VM_Operand *head_operand;

   vm_ident_t frame_id;
   VM_Stack *next_in_frame;
   size_t num_operands;
};


struct VM_Stack
{
   VM_Frame *root_frame;
   VM_Frame *head_frame;
   size_t num_frames;
};


struct VM_Thread
{
   vm_ident_t 	thread_id;
   VM_Operand 	*entry_point;
   VM_Operand 	*entry_args;


   enum
   {
     TH_RUNNING,
     TH_READY,
     TH_ZOMBIE,
     TH_DONE,
   }
   state;

   VM_Thread *next_in_sched;
};

struct VM_Sched
{
  VM_Thread 	*root_thread;
  VM_Thread 	*head_thread;
  size_t 	num_threads;
};

struct VM_Arena
{
   uint8_t 	*buffer;
   size_t 	capacity;
   size_t 	used;
   VM_Arena 	*next_in_memory;
};

struct VM_Memory
{
   VM_Arena *root;
   VM_Arena *head;
};

struct VM_State
{
   VM_Sched 	*sched;
   VM_Memory 	*memory;
   VM_Stack 	*stack;
};

void 		vm_frame_dump_all(VM_Frame *frame);
VM_Operand	*vm_frame_get_operand(VM_Frame *frame, vm_ident_t ident);

vm_ident_t 	vm_stack_stackup_frame(VM_Stack *stack);
VM_Frame	*vm_stack_get_frame(VM_Stack *stack, vm_ident_t frame_id);

vm_ident_t 	vm_stack_push_const(VM_Stack *stack, intmax_t const_value);
vm_ident_t 	vm_stack_push_addr(VM_Stack *stack, ptrdiff_t addr_value);
vm_ident_t 	vm_stack_push_mem(VM_Stack *stack, uintptr_t mem_value);
VM_Operand 	*vm_stack_pop(VM_Stack *stack);
VM_Operand 	*vm_stack_get(VM_Stack *stack, vm_ident_t operand_ident);
void 		vm_stack_dump(VM_Stack *stack);

VM_Arena 	*vm_arena_new(size_t size);
void 		vm_arena_dump_all(VM_Arena *arena);
uintptr_t 	vm_arena_allocate(VM_Arena *arena);

void 		vm_memory_init(VM_Memory *memory);
void 		vm_memory_cleanup(VM_Memory *memory);
uintptr_t 	vm_memory_request(VM_Memory *memory, size_t size);

void 		vm_sched_init(VM_Sched *sched);
VM_Thread 	*vm_thread_create(VM_Sched *sched, VM_Operand *entry_point, VM_Operand *entry_args);
void 		vm_schedule_threads(VM_Sched *sched);
void 		vm_sched_dump(VM_Sched *sched);

void		vm_state_init(VM_State *state);

#endif
