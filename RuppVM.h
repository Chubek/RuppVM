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
typedef uintmax_t vm_tag_t;

typedef struct VM_Operand 	VM_Operand;
typedef struct VM_Stack 	VM_Stack;
typedef struct VM_TestFn	VM_TestFn;
typedef struct VM_Frame		VM_Frame;
typedef struct VM_Thread 	VM_Thread;
typedef struct VM_Sched 	VM_Sched;
typedef struct VM_Arena 	VM_Arena;
typedef struct VM_Memory 	VM_Memory;
typedef struct VM_State 	VM_State;
typedef struct VM_Extern	VM_Extern;
typedef struct VM_Bundle	VM_Bundle;
typedef struct VM_Exec		VM_Exec;

typedef bool (*unfn_t)(VM_Operand*);
typedef bool (*binfn_t)(VM_Operand*, VM_Operand*);
typedef bool (*collfn_t)(VM_Operand*, size_t size);

struct VM_Operand
{
  union
  {
    uintptr_t 	mem_value;
    intmax_t 	const_value;
    ptrdiff_t 	addr_value;
    int64_t	inst_value;
  };

  enum
  {
    OP_MEM,
    OP_CONST,
    OP_ADDR,
    OP_INST,
  }
  kind;

  enum
  {
    ST_STATIC,
    ST_MEMORY,
    ST_THREAD_LOCAL,
  }
  storage;

  vm_ident_t 	operand_id;
  vm_tag_t	operand_tag;

  bool is_alive;
  
  VM_Operand *next_in_frame;
};

struct TestFn
{
   union
   {
      unfn_t 	*unary_fn;
      binfn_t	*binary_fn;
      collfn_t	*coll_fn;
   };

   enum
   {
       TST_UNARY,
       TST_BINARY,
       TST_COLL,
   }
   kind;
};

struct VM_Frame
{
   VM_Operand 	*root_operand;
   VM_Operand 	*head_operand;
   VM_Operand	**tos_operand;
   VM_Operand   **ip_operand;
   size_t 	num_operands;

   bool 	is_critial;
   vm_ident_t	current_holder;

   vm_ident_t 	frame_id;
   VM_Frame 	*next_in_stack;
};


struct VM_Stack
{
   VM_Frame 	*root_frame;
   VM_Frame 	*head_frame;
   VM_Operand 	**tos_operand;
   VM_Operand	**ip_operand;
   size_t 	num_frames;
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

   bool		global;
   VM_Thread 	*next_in_sched;
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
   size_t 	used_up;
   VM_Arena 	*next_in_memory;
};

struct VM_Memory
{
   VM_Arena *root_region;
   VM_Arena *head_region;
};

struct VM_Extern
{
   uintptr_t 	entrypoint;
   ssize_t 	num_params;
   bool 	returns;

   VM_Extern    *next_in_exec;
};

struct VM_Bundle
{
   VM_Opcode 	*start;
   VM_Opcode 	*end;
   size_t  	length;

   vm_ident_t	bundle_id;
   vm_tag_t 	bundle_tag;

   enum
   {
      BUN_STATIC,
      BUN_DYNAMIC,
   }
   kind;

   VM_Bundle 	*next;
};

struct VM_State
{
   VM_Sched 	*sched;
   VM_Memory 	*memory;
   VM_Stack 	*stack;
};

struct VM_Exec
{
   VM_State 	*state;
   VM_Extern	*hooked_externs;
   VM_Bundle	*bundles_tbl;
};

vm_ident_t	vm_ident_hash(void *addr, size_t size);

void 		vm_frame_dump_all(VM_Frame *frame);
VM_Operand	*vm_frame_get_operand(VM_Frame *frame, vm_ident_t ident);

vm_ident_t 	vm_stack_stackup_frame(VM_Stack *stack);
VM_Frame	*vm_stack_get_frame(VM_Stack *stack, vm_ident_t frame_id);

vm_ident_t 	vm_stack_push_const(VM_Stack *stack, intmax_t const_value, vm_tag_t tag);
vm_ident_t 	vm_stack_push_addr(VM_Stack *stack, ptrdiff_t addr_value, vm_tag_t tag);
vm_ident_t 	vm_stack_push_mem(VM_Stack *stack, uintptr_t mem_value, vm_tag_t tag);
vm_ident_t	vm_stack_push_inst(VM_Stack *stack, int64_t inst_value, vm_tag_t tag);
VM_Operand 	*vm_stack_pop(VM_Stack *stack);
VM_Operand 	*vm_stack_get(VM_Stack *stack, vm_ident_t ident);
void 		vm_stack_dump(VM_Stack *stack);

void		vm_stack_set_ip(VM_Stack *stack, VM_Operand *new_ip);
VM_Operand	*vm_stack_get_ip(VM_Stack *stack);
VM_Operand	*vm_stack_get_tos(VM_Stack *stack);

bool		vm_stack_test_tos(VM_Stack *stack, VM_TestFn *testfn);
bool		vm_stack_test_ident(VM_Stack *stack, VM_TestFn *testfn, vm_ident_t ident_start);

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
void 		vm_exec_init(VM_Exec *exec);

#endif
