#include "mmu.h"

#define NULL_ADDRESS	(0ul)
#define PHYS_MASK	(0xffful)
#define ENTRY_MASK	(0x1fful)
#define GB_MASK		(0x3ffffffful)
#define MB_MASK		(0x2fffful)

void StartNewPageTable(CPU *cpu)
{
    //Create cr3 all the way!

    //Take memory 1/2 up:
    ADDRESS p = (ADDRESS)&cpu->memory[cpu->mem_size / 2];
    cpu->cr3 = p + 0x1000 - (p & 0xfff);
}

//Write:
// virt_to_phys
// map
// unmap
ADDRESS virt_to_phys(CPU *cpu, ADDRESS virt)
{
    //Convert a physical address *virt* to
    //a physical address.

    //If the MMU is off, simply return the virtual address as
    //the physical address
    if (!(cpu->cr0 & (1 << 31))) {
        return virt;
    }
    if (cpu->cr3 == 0) {
        return RET_PAGE_FAULT;
    }

    return virt;
}

void map(CPU *cpu, ADDRESS phys, ADDRESS virt, PAGE_SIZE ps)
{
    //ALL PAGE MAPS must be located in cpu->memory!!

    //This function will return the PML4 of the mapped address

    //If the page size is 1G, you need a valid PML4 and PDP
    //If the page size is 2M, you need a valid PML4, PDP, and PD
    //If the page size is 4K, you need a valid PML4, PDP, PD, and PT
    //Remember that I could have some 2M pages and some 4K pages with a smattering
    //of 1G pages!

    if (cpu->cr3 == 0) {
        //Nothing has been created, so start here
        StartNewPageTable(cpu);
    }
    /*
       ADDRESS pml4e = (virt >> 39) & ENTRY_MASK;
       ADDRESS pdpe = (virt >> 30) & ENTRY_MASK;
       ADDRESS pde = (virt >> 21) & ENTRY_MASK;
       ADDRESS pte = (virt >> 12) & ENTRY_MASK;
       ADDRESS p = virt & PHYS_MASK;

       ADDRESS *pml4 = (ADDRESS *)cpu->cr3;
       ADDRESS *pdp;
       ADDRESS *pd;
       ADDRESS *pt;
       */
}

void unmap(CPU *cpu, ADDRESS virt, PAGE_SIZE ps)
{
    //Simply set the present bit (P) to 0 of the virtual address page
    //If the page size is 1G, set the present bit of the PDP to 0
    //If the page size is 2M, set the present bit of the PD  to 0
    //If the page size is 4K, set the present bit of the PT  to 0


    if (cpu->cr3 == 0)
        return;

}



//////////////////////////////////////////
//
//Do not touch the functions below!
//
//////////////////////////////////////////
void cpu_pfault(CPU *cpu)
{
    printf("MMU: #PF @ 0x%016lx\n", cpu->cr2);
}


CPU *new_cpu(unsigned int mem_size)
{
    CPU *ret;


    ret = (CPU*)calloc(1, sizeof(CPU));
    ret->memory = (char*)calloc(mem_size, sizeof(char));
    ret->mem_size = mem_size;

#if defined(I_DID_EXTRA_CREDIT)
    ret->tlb = (TLB_ENTRY*)calloc(TLB_SIZE, sizeof(TLB_ENTRY));
#endif

    return ret;
}

void destroy_cpu(CPU *cpu)
{
    if (cpu->memory) {
        free(cpu->memory);
    }
#if defined(I_DID_EXTRA_CREDIT)
    if (cpu->tlb) {
        free(cpu->tlb);
    }
    cpu->tlb = 0;
#endif

    cpu->memory = 0;

    free(cpu);
}


int mem_get(CPU *cpu, ADDRESS virt)
{
    ADDRESS phys = virt_to_phys(cpu, virt);
    if (phys == RET_PAGE_FAULT || phys + 4 >= cpu->mem_size) {
        cpu->cr2 = virt;
        cpu_pfault(cpu);
        return -1;
    }
    else {
        return *((int*)(&cpu->memory[phys]));
    }
}

void mem_set(CPU *cpu, ADDRESS virt, int value)
{
    ADDRESS phys = virt_to_phys(cpu, virt);
    if (phys == RET_PAGE_FAULT || phys + 4 >= cpu->mem_size) {
        cpu->cr2 = virt;
        cpu_pfault(cpu);
    }
    else {
        *((int*)(&cpu->memory[phys])) = value;
    }
}

#if defined(I_DID_EXTRA_CREDIT)

void print_tlb(CPU *cpu)
{
    int i;
    TLB_ENTRY *entry;

    printf("#   %-18s %-18s Tag\n", "Virtual", "Physical");
    for (i = 0;i < TLB_SIZE;i++) {
        entry = &cpu->tlb[i];
        printf("%-3d 0x%016lx 0x%016lx %08x\n", i+1, entry->virt, entry->phys, entry->tag);
    }
}

#endif
