

/*****************************************************************************
 * Communication Functions
 ****************************************************************************/

/*
 * Write a sequence of bytes.
 *
 * Returns:
 *    0   if successful
 *    GDB_EOF if failed to write all bytes
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

/*
 * Read a sequence of bytes.
 *
 * Returns:
 *    0   if successfully read len bytes
 *    GDB_EOF if failed to read all bytes
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
