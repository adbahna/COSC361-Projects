#include "mmu.h"

#define NULL_ADDRESS    (0ul)
#define PHYS_MASK       (0xffful)
#define ENTRY_MASK      (0x1fful)
#define GB_MASK         (0x3ffffffful)
#define MB_MASK         (0x2fffful)

typedef struct {
    ADDRESS* entries[512];
} Table;

void StartNewPageTable(CPU *cpu)
{
    //Create cr3 all the way!

    //Take memory 1/2 up:
    ADDRESS p = (ADDRESS)&cpu->memory[cpu->mem_size / 2];
    cpu->cr3 = p + 0x1000 - (p & 0xfff);
}

ADDRESS virt_to_phys(CPU *cpu, ADDRESS virt)
{
    //If the MMU is off, simply return the virtual address as
    //the physical address
    if (!(cpu->cr0 & (1 << 31))) {
        return virt;
    }
    if (cpu->cr3 == 0) {
        return RET_PAGE_FAULT;
    }

    ADDRESS pml4, pdp, pd, pt, p;
    ADDRESS pml4e, pdpe, pde, pte, pe;

    //Convert a physical address *virt* to
    //a physical address.

    pml4 = (cpu->cr3 >> 12) << 12;
    pml4e = pml4 + ((virt >> 39) & ENTRY_MASK);
    pml4e = *((ADDRESS*)&cpu->memory[pml4e]);
    //If the P bit is 0, page fault
    if ((pml4e & 0x1) == 0)
        return RET_PAGE_FAULT;

    pdp = (pml4e >> 12) << 12;
    pdpe = pdp + ((virt >> 30) & ENTRY_MASK);
    pdpe = *((ADDRESS*)&cpu->memory[pdpe]);
    //If the P bit is 0, page fault
    if ((pdpe & 0x1) == 0)
        return RET_PAGE_FAULT;
    //If the PS bit is 0 on the PDPE, then we have a 1GB page size
    if (((pdpe >> 7) & 0x1) != 0) {
        p = (pdpe >> 12) << 12;
        pe = p + (virt & GB_MASK);
        return *((ADDRESS*)&cpu->memory[pe]);
    }

    pd = (pdpe >> 12) << 12;
    pde = pd + ((virt >> 21) & ENTRY_MASK);
    pde = *((ADDRESS*)&cpu->memory[pde]);
    //If the P bit is 0, page fault
    if ((pde & 0x1) == 0)
        return RET_PAGE_FAULT;
    //If the PS bit is 0 on the PDE, then we have a 2MB page size
    if (((pde >> 7) & 0x1) != 0) {
        p = (pde >> 12) << 12;
        pe = p + (virt & MB_MASK);
        return *((ADDRESS*)&cpu->memory[pe]);
    }

    pt = (pde >> 12) << 12;
    pte = pt + ((virt >> 12) & ENTRY_MASK);
    pte = *((ADDRESS*)&cpu->memory[pte]);
    //If the P bit is 0, page fault
    if ((pte & 0x1) == 0)
        return RET_PAGE_FAULT;

    //We have a 4KB page size
    p = (pte >> 12) << 12;
    pe = p + (virt & PHYS_MASK);

    return *((ADDRESS*)&cpu->memory[pe]);
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
    /*
    ADDRESS pml4e = (virt >> 39) & ENTRY_MASK;
    ADDRESS pdpe = (virt >> 30) & ENTRY_MASK;
    ADDRESS pde = (virt >> 21) & ENTRY_MASK;
    ADDRESS pte = (virt >> 12) & ENTRY_MASK;
    ADDRESS pe = virt & PHYS_MASK;

    ADDRESS *pml4 = (ADDRESS *)cpu->cr3;
    ADDRESS *pdp;
    ADDRESS *pd;
    ADDRESS *pt;
    */

    if (cpu->cr3 == 0) StartNewPageTable(cpu);

    if (cpu->cr3 == 0) return;

    if (ps == PS_4K) {

    } else if (ps == PS_2M) {

    } else if (ps == PS_1G) {

    }
}

void unmap(CPU *cpu, ADDRESS virt, PAGE_SIZE ps)
{
    if (cpu->cr3 == 0)
        return;

    ADDRESS pml4, pdp, pd, pt;
    ADDRESS pml4e, pdpe, pde, pte;

    // get the pml4 base address from cr3
    pml4 = (cpu->cr3 >> 12) << 12;

    //If the page size is 1G, set the present bit of the PDP to 0
    if (ps == PS_1G) {
        pml4e = pml4 + ((virt >> 39) & ENTRY_MASK);
        pml4e = *((ADDRESS*)&cpu->memory[pml4e]);
        //If the P bit is 0, return because the map is invalid
        if ((pml4e & 0x1) == 0)
            return;

        pdp = (pml4e >> 12) << 12;
        pdpe = pdp + ((virt >> 30) & ENTRY_MASK);

        //Simply set the present bit (P) to 0 of the virtual address page
        *((ADDRESS*)&cpu->memory[pdpe]) = *((ADDRESS*)&cpu->memory[pdpe]) & 0xfffffffffffffffe;
    }
    //If the page size is 2M, set the present bit of the PD  to 0
    else if (ps == PS_2M) {
        pml4e = pml4 + ((virt >> 39) & ENTRY_MASK);
        pml4e = *((ADDRESS*)&cpu->memory[pml4e]);
        //If the P bit is 0, return because the map is invalid
        if ((pml4e & 0x1) == 0)
            return;

        pdp = (pml4e >> 12) << 12;
        pdpe = pdp + ((virt >> 30) & ENTRY_MASK);
        pdpe = *((ADDRESS*)&cpu->memory[pdpe]);
        //If the P bit or PS bit is 0, the map is invalid
        if (((pdpe & 0x1) == 0) && (((pdpe >> 7) & 0x1) != 0))
            return;

        pd = (pdpe >> 12) << 12;
        pde = pd + ((virt >> 21) & ENTRY_MASK);

        //Simply set the present bit (P) to 0 of the virtual address page
        *((ADDRESS*)&cpu->memory[pde]) = *((ADDRESS*)&cpu->memory[pde]) & 0xfffffffffffffffe;
    }
    //If the page size is 4K, set the present bit of the PT  to 0
    else {
        pml4e = pml4 + ((virt >> 39) & ENTRY_MASK);
        pml4e = *((ADDRESS*)&cpu->memory[pml4e]);
        //If the P bit is 0, return because the map is invalid
        if ((pml4e & 0x1) == 0)
            return;

        pdp = (pml4e >> 12) << 12;
        pdpe = pdp + ((virt >> 30) & ENTRY_MASK);
        pdpe = *((ADDRESS*)&cpu->memory[pdpe]);
        //If the P bit or PS bit is 0, the map is invalid
        if (((pdpe & 0x1) == 0) && (((pdpe >> 7) & 0x1) != 0))
            return;

        pd = (pdpe >> 12) << 12;
        pde = pd + ((virt >> 21) & ENTRY_MASK);
        pde = *((ADDRESS*)&cpu->memory[pde]);
        //If the P bit or PS bit is 0, the map is invalid
        if (((pde & 0x1) == 0) && (((pde >> 7) & 0x1) != 0))
            return;

        pt = (pde >> 12) << 12;
        pte = pt + ((virt >> 12) & ENTRY_MASK);
        pte = *((ADDRESS*)&cpu->memory[pte]);

        //Simply set the present bit (P) to 0 of the virtual address page
        *((ADDRESS*)&cpu->memory[pte]) = *((ADDRESS*)&cpu->memory[pte]) & 0xfffffffffffffffe;
    }
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
