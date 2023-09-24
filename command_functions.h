/*****************************************************************************
 * Command Functions
 ****************************************************************************/

/**
 * @brief Read from memory and encode into buf.
 * 
 * @param state Pointer to the GDB state object
 * @param buf Buffer to write the encoded data to
 * @param buf_len Length of the buffer
 * @param addr Memory address to read from
 * @param len Number of bytes to read
 * @param enc Encoding function
 * 
 * @return Number of bytes written to buf, or GDB_EOF if the buffer is too small
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

/**
 * @brief Write to memory from encoded buf.
 * 
 * @param state Pointer to the GDB state object
 * @param buf Encoded buffer to read data from
 * @param buf_len Length of the buffer
 * @param addr Memory address to write to
 * @param len Number of bytes to write
 * @param dec Decoding function
 * 
 * @return 0 on success, or GDB_EOF on failure
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

/**
 * @brief Continue program execution at PC.
 * 
 * @param state Pointer to the GDB state object
 * 
 * @return 0 on success
 */
int gdb_continue(struct gdb_state *state)
{
    gdb_sys_continue(state);
    return 0;
}

/**
 * @brief Step one instruction in the program.
 * 
 * @param state Pointer to the GDB state object
 * 
 * @return 0 on success
 */
int gdb_step(struct gdb_state *state)
{
    gdb_sys_step(state);
    return 0;
}
