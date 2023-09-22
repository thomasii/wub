/*****************************************************************************
 * Command Functions
 ****************************************************************************/

/*
 * Read from memory and encode into buf.
 *
 * Returns
 *    0+  number of bytes written to buf
 *    GDB_EOF if the buffer is too small
 */
static int gdb_mem_read(struct gdb_state *state, char *buf,
                        unsigned int buf_len, address addr, unsigned int len,
                        gdb_enc_func enc)
{
    char data[64];
    unsigned int pos;

    if (len > sizeof(data)) {
        return GDB_EOF;
    }

    /* Read from system memory */
    for (pos = 0; pos < len; pos++) {
        if (gdb_sys_mem_readb(state, addr+pos, &data[pos])) {
            /* Failed to read */
            return GDB_EOF;
        }
    }

    /* Encode data */
    return enc(buf, buf_len, data, len);
}

/*
 * Write to memory from encoded buf.
 */
static int gdb_mem_write(struct gdb_state *state, const char *buf,
                         unsigned int buf_len, address addr, unsigned int len,
                         gdb_dec_func dec)
{
    char data[64];
    unsigned int pos;

    if (len > sizeof(data)) {
        return GDB_EOF;
    }

    /* Decode data */
    if (dec(buf, buf_len, data, len) == GDB_EOF) {
        return GDB_EOF;
    }

    /* Write to system memory */
    for (pos = 0; pos < len; pos++) {
        if (gdb_sys_mem_writeb(state, addr+pos, data[pos])) {
            /* Failed to write */
            return GDB_EOF;
        }
    }

    return 0;
}

/*
 * Continue program execution at PC.
 */
int gdb_continue(struct gdb_state *state)
{
    gdb_sys_continue(state);
    return 0;
}

/*
 * Step one instruction.
 */
int gdb_step(struct gdb_state *state)
{
    gdb_sys_step(state);
    return 0;
}
