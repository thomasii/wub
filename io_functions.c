/*****************************************************************************
 * I/O Functions
 ****************************************************************************/

/*
 * Write to I/O port.
 */
static void gdb_x86_io_write_8(uint16_t port, uint8_t val)
{
    asm volatile (
        "outb    %%al, %%dx;"
        /* Outputs  */ : /* None */
        /* Inputs   */ : "a" (val), "d" (port)
        /* Clobbers */ : /* None */
        );
}

/*
 * Read from I/O port.
 */
static uint8_t gdb_x86_io_read_8(uint16_t port)
{
    uint8_t val;

    asm volatile (
        "inb     %%dx, %%al;"
        /* Outputs  */ : "=a" (val)
        /* Inputs   */ : "d" (port)
        /* Clobbers */ : /* None */
        );

    return val;
}
