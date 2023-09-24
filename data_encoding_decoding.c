/*****************************************************************************
 * Data Encoding/Decoding Functions
 ****************************************************************************/

/**
 * @brief Encode data into its hexadecimal representation and store in a buffer.
 * 
 * @param buf Output buffer where the hexadecimal representation will be stored.
 * @param buf_len Size of the output buffer.
 * @param data Input data buffer to encode.
 * @param data_len Length of the input data.
 * 
 * @return The number of bytes written to buf or GDB_EOF if the buffer is too small.
 */
static int gdb_enc_hex(char *buf, unsigned int buf_len, const char *data,
                       unsigned int data_len)
{
    unsigned int pos;

    if (buf_len < data_len*2) {
        /* Buffer too small */
        return GDB_EOF;
    }

    for (pos = 0; pos < data_len; pos++) {
        *buf++ = gdb_get_digit((data[pos] >> 4) & 0xf);
        *buf++ = gdb_get_digit((data[pos]     ) & 0xf);
    }

    return data_len*2;
}

/**
 * @brief Decode data from its hexadecimal representation and store in a buffer.
 * 
 * @param buf Input buffer containing the hexadecimal data.
 * @param buf_len Length of the input buffer.
 * @param data Output buffer where the decoded data will be stored.
 * @param data_len Expected length of the decoded data.
 * 
 * @return 0 if decoding was successful, or GDB_EOF if the buffer is too small or contains invalid data.
 */
static int gdb_dec_hex(const char *buf, unsigned int buf_len, char *data,
                       unsigned int data_len)
{
    unsigned int pos;
    int tmp;

    if (buf_len != data_len*2) {
        /* Buffer too small */
        return GDB_EOF;
    }

    for (pos = 0; pos < data_len; pos++) {
        /* Decode high nibble */
        tmp = gdb_get_val(*buf++, 16);
        if (tmp == GDB_EOF) {
            /* Buffer contained junk */
            GDB_ASSERT(0);
            return GDB_EOF;
        }

        data[pos] = tmp << 4;

        /* Decode low nibble */
        tmp = gdb_get_val(*buf++, 16);
        if (tmp == GDB_EOF) {
            /* Buffer contained junk */
            GDB_ASSERT(0);
            return GDB_EOF;
        }
        data[pos] |= tmp;
    }

    return 0;
}

/**
 * @brief Encode data into its binary representation and store in a buffer.
 * 
 * @param buf Output buffer where the binary representation will be stored.
 * @param buf_len Size of the output buffer.
 * @param data Input data buffer to encode.
 * @param data_len Length of the input data.
 * 
 * @return The number of bytes written to buf or GDB_EOF if the buffer is too small.
 */
static int gdb_enc_bin(char *buf, unsigned int buf_len, const char *data,
                       unsigned int data_len)
{
    unsigned int buf_pos, data_pos;

    for (buf_pos = 0, data_pos = 0; data_pos < data_len; data_pos++) {
        if (data[data_pos] == '$' ||
            data[data_pos] == '#' ||
            data[data_pos] == '}' ||
            data[data_pos] == '*') {
            if (buf_pos+1 >= buf_len) {
                GDB_ASSERT(0);
                return GDB_EOF;
            }
            buf[buf_pos++] = '}';
            buf[buf_pos++] = data[data_pos] ^ 0x20;
        } else {
            if (buf_pos >= buf_len) {
                GDB_ASSERT(0);
                return GDB_EOF;
            }
            buf[buf_pos++] = data[data_pos];
        }
    }

    return buf_pos;
}

/**
 * @brief Decode data from its binary representation and store in a buffer.
 * 
 * @param buf Input buffer containing the binary data.
 * @param buf_len Length of the input buffer.
 * @param data Output buffer where the decoded data will be stored.
 * @param data_len Expected length of the decoded data.
 * 
 * @return The number of bytes decoded or GDB_EOF if the buffer is too small or contains invalid data.
 */
static int gdb_dec_bin(const char *buf, unsigned int buf_len, char *data,
                       unsigned int data_len)
{
    unsigned int buf_pos, data_pos;

    for (buf_pos = 0, data_pos = 0; buf_pos < buf_len; buf_pos++) {
        if (data_pos >= data_len) {
            /* Output buffer overflow */
            GDB_ASSERT(0);
            return GDB_EOF;
        }
        if (buf[buf_pos] == '}') {
            /* The next byte is escaped */
            if (buf_pos+1 >= buf_len) {
                /* No escaped character following the escape character */
                GDB_ASSERT(0);
                return GDB_EOF;
            }
            buf_pos += 1;
            data[data_pos++] = buf[buf_pos] ^ 0x20;
        } else {
            data[data_pos++] = buf[buf_pos];
        }
    }

    return data_pos;
}
