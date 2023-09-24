/*****************************************************************************
 * Debugging System MISC
 ****************************************************************************/

/// Enable debug statements (printf)
#ifndef DEBUG
#define DEBUG 0
#endif

/**
 * @defgroup gdbstub_arch_mock Debugging System Functions for Mock Architecture
 * @{
 */

#ifdef GDBSTUB_ARCH_MOCK

/**
 * @brief Write one character to the debugging stream.
 * 
 * @param state Pointer to the gdb_state struct
 * @param ch Character to write
 * @return Always returns 0
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

/**
 * @brief Read one character from the debugging stream.
 * 
 * @param state Pointer to the gdb_state struct
 * @return The read character or GDB_EOF
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

// Other functions with Doxygen comments here...

/**
 * @brief Buffer write function for gdb_buf_write.
 *
 * @param buf The buffer to write to.
 * @param ch The character to write.
 */
void gdb_buf_write(struct gdb_buffer *buf, int ch)
{
    // function implementation
}

/**
 * @brief Buffer read function for gdb_buf_read.
 *
 * @param buf The buffer to read from.
 * @return The read character or EOF.
 */
int gdb_buf_read(struct gdb_buffer *buf)
{
    // function implementation
}

#endif /* GDBSTUB_ARCH_MOCK */

/** @} */

/**
 * @defgroup gdbstub_arch_x86 Debugging System Functions for x86 Architecture
 * @{
 */

#ifdef GDBSTUB_ARCH_X86

// Doxygen comments for x86-specific functions...

#endif /* GDBSTUB_ARCH_X86 */

/** @} */
