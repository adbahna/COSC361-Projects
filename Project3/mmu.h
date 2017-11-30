//mmu.h - Memory Management Unit Project (COSC 361)
//Stephen Marz 2017
#pragma once
#include <stdio.h>
#include <stdlib.h>

//Uncomment the following line if you're
//doing the extra credit

//#define I_DID_EXTRA_CREDIT

#define REGISTER unsigned long
#define ADDRESS unsigned long

#if defined(I_DID_EXTRA_CREDIT)
//EXTRA CREDIT: TLB stuff
//TLB_SIZE defines the number of TLB entries
#define TLB_SIZE 16

typedef struct
{
    ADDRESS phys;
    ADDRESS virt;
    unsigned int tag;
} TLB_ENTRY;

typedef enum {
    RP_FIFO = 0, // first-in, first-out
    RP_SC, // second chance
    RP_LRU, // least recently used
    RP_LFU, // least frequently used
    RP_CLOCK, // clock algorithm
} TLB_REPLACEMENT_POLICY;

#endif

#define RET_PAGE_FAULT (-1ul)

typedef enum {
    PS_4K,
    PS_2M,
    PS_1G
} PAGE_SIZE;

typedef struct
{
    //Control Register 0
    //for enabling/disabling the MMU
    REGISTER cr0;
    //Control Register 2
    //for storing the page fault address
    REGISTER cr2;
    //Control Register 3
    //for storing the PML4 base address
    REGISTER cr3;

    //The RAM of this CPU
    char *memory;
    //The size of the memory region
    unsigned int mem_size;

#if defined(I_DID_EXTRA_CREDIT)
    //EXTRA CREDIT: TLB
    //TLB_ENTRY *tlb is a one-dimensionaly array
    //that has a size of TLB_SIZE. You can index
    //a pointer just like an array:
    //tlb[0] is the first TLB_ENTRY
    //tlb[0].virt gives the virtual address of
    //the first TLB_ENTRY
    TLB_ENTRY *tlb;
    TLB_REPLACEMENT_POLICY tlb_policy;
#endif
} CPU;

//Convert a virtual address (address) into a physical address
ADDRESS virt_to_phys(CPU *cpu, ADDRESS virt);

//Map a physical address to a virtual one at the given page size
//This is required to set the cr3 if it isn't valid. For your
//very first map, cpu->cr3 will be 0, which signals that it is
//invalid. You are required to point it to a valid memory location
//in the second half of cpu->memory
void map(CPU *cpu, ADDRESS phys, ADDRESS virt, PAGE_SIZE ps);

//Unmap a virtual address at the given page size
void unmap(CPU *cpu, ADDRESS virt, PAGE_SIZE ps);

#if defined(I_DID_EXTRA_CREDIT)
//EXTRA CREDIT: Invalidate a TLB entry by virtual address
void invlpg(CPU *cpu, ADDRESS virt);
#endif

//////////////////////////////////////////////////////
//
// Already defined functions!
//
////////////////////////////////////////////////////
//A page fault
void cpu_pfault(CPU *cpu);

//Create a new CPU or destroy one
CPU *new_cpu(unsigned int memsize);
void destroy_cpu(CPU *cpu);

//Get or set a value at a virtual memory address
int mem_get(CPU *cpu, ADDRESS virt);
void mem_set(CPU *cpu, ADDRESS virt, int value);

#if defined(I_DID_EXTRA_CREDIT)
//Print all of the TLB entries
void print_tlb(CPU *cpu);
#endif
