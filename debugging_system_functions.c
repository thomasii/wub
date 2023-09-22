/*****************************************************************************
 * Debugging System MISC
 ****************************************************************************/

/* Enable debug statements (printf) */
#ifndef DEBUG
#define DEBUG 0
#endif

/*****************************************************************************
 * Debugging System Functions
 ****************************************************************************/

#ifdef GDBSTUB_ARCH_MOCK

/*
 * Write one character to the debugging stream.
 */
int gdb_sys_putchar(struct gdb_state *state, int ch)
{
#ifdef USE_STDIO
    putchar(ch);
#else
    gdb_buf_write(&gdb_output, ch);
#endif
    return 0;
}

/*
 * Read one character from the debugging stream.
 */
int gdb_sys_getc(struct gdb_state *state)
{
#ifdef USE_STDIO
    int ch = getchar();
    return ch == EOF ? GDB_EOF : ch;
#else
    return gdb_buf_read(&gdb_input);
#endif
}

/*
 * Read one byte from memory.
 */
int gdb_sys_mem_readb(struct gdb_state *state, address addr, char *val)
{
    if (addr >= sizeof(gdb_mem)) {
        return 1;
    }

    *val = gdb_mem[addr];
    return 0;
}

/*
 * Write one byte to memory.
 */
int gdb_sys_mem_writeb(struct gdb_state *state, address addr, char val)
{
    if (addr >= sizeof(gdb_mem)) {
        return 1;
    }

    gdb_mem[addr] = val;
    return 0;
}

/*
 * Continue program execution.
 */
int gdb_sys_continue(struct gdb_state *state)
{
    return 0;
}

/*
 * Single step the next instruction.
 */
int gdb_sys_step(struct gdb_state *state)
{
    return 0;
}


void gdb_buf_write(struct gdb_buffer *buf, int ch)
{
    if (buf->buf == NULL) {
        buf->pos_write = 0;
        buf->pos_read = 0;
        buf->size = 512;
        buf->buf = malloc(buf->size);
    } else if (buf->pos_write >= buf->size) {
        buf->size *= 2;
        buf->buf = realloc(buf->buf, buf->size);
    }

    assert(buf->buf);
    buf->buf[buf->pos_write++] = ch;
}

int gdb_buf_read(struct gdb_buffer *buf)
{
    if (buf->buf && buf->pos_read < buf->pos_write) {
        return buf->buf[buf->pos_read++];
    }
    return EOF;
}

#endif /* GDBSTUB_ARCH_MOCK */


#ifdef GDBSTUB_ARCH_X86

/*****************************************************************************
 * Debugging System Functions
 ****************************************************************************/

/*
 * Write one character to the debugging stream.
 */
int gdb_sys_putchar(struct gdb_state *state, int ch)
{
    return gdb_x86_serial_putchar(ch);
}

/*
 * Read one character from the debugging stream.
 */
int gdb_sys_getc(struct gdb_state *state)
{
    return gdb_x86_serial_getc() & 0xff;
}

/*
 * Read one byte from memory.
 */
int gdb_sys_mem_readb(struct gdb_state *state, address addr, char *val)
{
    *val = *(volatile char *)addr;
    return 0;
}

/*
 * Write one byte to memory.
 */
int gdb_sys_mem_writeb(struct gdb_state *state, address addr, char val)
{
    *(volatile char *)addr = val;
    return 0;
}

/*
 * Continue program execution.
 */
int gdb_sys_continue(struct gdb_state *state)
{
    gdb_state.registers[GDB_CPU_I386_REG_PS] &= ~(1<<8);
    return 0;
}

/*
 * Single step the next instruction.
 */
int gdb_sys_step(struct gdb_state *state)
{
    gdb_state.registers[GDB_CPU_I386_REG_PS] |= 1<<8;
    return 0;
}

/*
 * Debugger init function.
 *
 * Hooks the IDT to enable debugging.
 */
void gdb_sys_init(void)
{
    /* Hook current IDT. */
    gdb_x86_hook_idt(1, gdb_x86_int_handlers[1]);
    gdb_x86_hook_idt(3, gdb_x86_int_handlers[3]);

    /* Interrupt to start debugging. */
    asm volatile ("int3");
}



#endif /* GDBSTUB_ARCH_X86 */
