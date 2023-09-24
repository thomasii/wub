/*****************************************************************************
 * Communication Functions
 ****************************************************************************/

/**
 * @brief Write a sequence of bytes to a specified output.
 *
 * @param state Pointer to the GDB state object.
 * @param buf Pointer to the buffer containing the bytes to be written.
 * @param len The number of bytes to write.
 *
 * @return 0 if all bytes were successfully written, or GDB_EOF if an error occurred.
 */
static int gdb_write(struct gdb_state *state, const char *buf, unsigned int len)
{
    while (len--) {
        if (gdb_sys_putchar(state, *buf++) == GDB_EOF) {
            return GDB_EOF;
        }
    }

    return 0;
}

/**
 * @brief Read a sequence of bytes into a buffer.
 *
 * @param state Pointer to the GDB state object.
 * @param buf Pointer to the buffer where the bytes will be read into.
 * @param buf_len Length of the buffer.
 * @param len The number of bytes to read.
 *
 * @return 0 if exactly len bytes were successfully read, or GDB_EOF if an error occurred or the buffer is too small.
 */
static int gdb_read(struct gdb_state *state, char *buf, unsigned int buf_len,
                    unsigned int len)
{
    char c;

    if (buf_len < len) {
        /* Buffer too small */
        return GDB_EOF;
    }

    while (len--) {
        if ((c = gdb_sys_getc(state)) == GDB_EOF) {
            return GDB_EOF;
        }
        *buf++ = c;
    }

    return 0;
}
