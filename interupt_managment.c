/*
 * Copyright (c) 2016-2022 Matt Borgerson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*****************************************************************************
 * Interrupt Management Structs / Types
 ****************************************************************************/

#pragma pack(1)

struct gdb_interrupt_state {
    uint32_t ss;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t vector;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

struct gdb_idtr
{
    uint16_t len;
    uint32_t offset;
};

struct gdb_idt_gate
{
    uint16_t offset_low;
    uint16_t segment;
    uint16_t flags;
    uint16_t offset_high;
};

#pragma pack()

/*****************************************************************************
 * Interrupt Management Const Data
 ****************************************************************************/

extern void const * const gdb_x86_int_handlers[];

/*****************************************************************************
 * Interrupt Management Functions
 ****************************************************************************/

/*
 * Initialize idt_gates with the interrupt handlers.
 */
static void gdb_x86_init_gates(void)
{
    unsigned int i;
    uint16_t cs;

    cs = gdb_x86_get_cs();
    for (i = 0; i < NUM_IDT_ENTRIES; i++) {
        gdb_idt_gates[i].flags       = 0x8E00;
        gdb_idt_gates[i].segment     = cs;
        gdb_idt_gates[i].offset_low  =
            ((uint32_t)gdb_x86_int_handlers[i]      ) & 0xffff;
        gdb_idt_gates[i].offset_high =
            ((uint32_t)gdb_x86_int_handlers[i] >> 16) & 0xffff;
    }
}

/*
 * Load a new IDT.
 */
static void gdb_x86_load_idt(struct gdb_idtr *idtr)
{
    asm volatile (
        "lidt    %0"
        /* Outputs  */ : /* None */
        /* Inputs   */ : "m" (*idtr)
        /* Clobbers */ : /* None */
        );
}

/*
 * Get current IDT.
 */
static void gdb_x86_store_idt(struct gdb_idtr *idtr)
{
    asm volatile (
        "sidt    %0"
        /* Outputs  */ : "=m" (*idtr)
        /* Inputs   */ : /* None */
        /* Clobbers */ : /* None */
        );
}

/*
 * Hook a vector of the current IDT.
 */
static void gdb_x86_hook_idt(uint8_t vector, const void *function)
{
    struct gdb_idtr      idtr;
    struct gdb_idt_gate *gates;

    gdb_x86_store_idt(&idtr);
    gates = (struct gdb_idt_gate *)idtr.offset;
    gates[vector].flags       = 0x8E00;
    gates[vector].segment     = gdb_x86_get_cs();
    gates[vector].offset_low  = (((uint32_t)function)      ) & 0xffff;
    gates[vector].offset_high = (((uint32_t)function) >> 16) & 0xffff;
}

/*
 * Initialize IDT gates and load the new IDT.
 */
static void gdb_x86_init_idt(void)
{
    struct gdb_idtr idtr;

    gdb_x86_init_gates();
    idtr.len = sizeof(gdb_idt_gates)-1;
    idtr.offset = (uint32_t)gdb_idt_gates;
    gdb_x86_load_idt(&idtr);
}

/*
 * Common interrupt handler routine.
 */
void gdb_x86_int_handler(struct gdb_interrupt_state *istate)
{
    gdb_x86_interrupt(istate);
}

/*
 * Debug interrupt handler.
 */
static void gdb_x86_interrupt(struct gdb_interrupt_state *istate)
{
    /* Translate vector to signal */
    switch (istate->vector) {
    case 1:  gdb_state.signum = 5; break;
    case 3:  gdb_state.signum = 5; break;
    default: gdb_state.signum = 7;
    }

    /* Load Registers */
    gdb_state.registers[GDB_CPU_I386_REG_EAX] = istate->eax;
    gdb_state.registers[GDB_CPU_I386_REG_ECX] = istate->ecx;
    gdb_state.registers[GDB_CPU_I386_REG_EDX] = istate->edx;
    gdb_state.registers[GDB_CPU_I386_REG_EBX] = istate->ebx;
    gdb_state.registers[GDB_CPU_I386_REG_ESP] = istate->esp;
    gdb_state.registers[GDB_CPU_I386_REG_EBP] = istate->ebp;
    gdb_state.registers[GDB_CPU_I386_REG_ESI] = istate->esi;
    gdb_state.registers[GDB_CPU_I386_REG_EDI] = istate->edi;
    gdb_state.registers[GDB_CPU_I386_REG_PC]  = istate->eip;
    gdb_state.registers[GDB_CPU_I386_REG_CS]  = istate->cs;
    gdb_state.registers[GDB_CPU_I386_REG_PS]  = istate->eflags;
    gdb_state.registers[GDB_CPU_I386_REG_SS]  = istate->ss;
    gdb_state.registers[GDB_CPU_I386_REG_DS]  = istate->ds;
    gdb_state.registers[GDB_CPU_I386_REG_ES]  = istate->es;
    gdb_state.registers[GDB_CPU_I386_REG_FS]  = istate->fs;
    gdb_state.registers[GDB_CPU_I386_REG_GS]  = istate->gs;

    gdb_main(&gdb_state); // Not sure if this will cause problems seperated in h file here.

    /* Restore Registers */
    istate->eax    = gdb_state.registers[GDB_CPU_I386_REG_EAX];
    istate->ecx    = gdb_state.registers[GDB_CPU_I386_REG_ECX];
    istate->edx    = gdb_state.registers[GDB_CPU_I386_REG_EDX];
    istate->ebx    = gdb_state.registers[GDB_CPU_I386_REG_EBX];
    istate->esp    = gdb_state.registers[GDB_CPU_I386_REG_ESP];
    istate->ebp    = gdb_state.registers[GDB_CPU_I386_REG_EBP];
    istate->esi    = gdb_state.registers[GDB_CPU_I386_REG_ESI];
    istate->edi    = gdb_state.registers[GDB_CPU_I386_REG_EDI];
    istate->eip    = gdb_state.registers[GDB_CPU_I386_REG_PC];
    istate->cs     = gdb_state.registers[GDB_CPU_I386_REG_CS];
    istate->eflags = gdb_state.registers[GDB_CPU_I386_REG_PS];
    istate->ss     = gdb_state.registers[GDB_CPU_I386_REG_SS];
    istate->ds     = gdb_state.registers[GDB_CPU_I386_REG_DS];
    istate->es     = gdb_state.registers[GDB_CPU_I386_REG_ES];
    istate->fs     = gdb_state.registers[GDB_CPU_I386_REG_FS];
    istate->gs     = gdb_state.registers[GDB_CPU_I386_REG_GS];
}
