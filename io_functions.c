/*****************************************************************************
 * I/O Functions
 ****************************************************************************/

/**
 * @brief Write an 8-bit value to a specified I/O port.
 *
 * @param port The I/O port to write to.
 * @param val The 8-bit value to write.
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

/**
 * @brief Read an 8-bit value from a specified I/O port.
 *
 * @param port The I/O port to read from.
 * @return The 8-bit value read from the I/O port.
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
