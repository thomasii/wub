

/*****************************************************************************
 * Packet Creation Helpers
 ****************************************************************************/

/*
 * Send OK packet
 */
static int gdb_send_ok_packet(struct gdb_state *state, char *buf,
                              unsigned int buf_len)
{
    return gdb_send_packet(state, "OK", 2);
}

/*
 * Send a message to the debugging console (via O XX... packet)
 */
static int gdb_send_conmsg_packet(struct gdb_state *state, char *buf,
                                  unsigned int buf_len, const char *msg)
{
    unsigned int size;
    int status;

    if (buf_len < 2) {
        /* Buffer too small */
        return GDB_EOF;
    }

    buf[0] = 'O';
    status = gdb_enc_hex(&buf[1], buf_len-1, msg, gdb_strlen(msg));
    if (status == GDB_EOF) {
        return GDB_EOF;
    }
    size = 1 + status;
    return gdb_send_packet(state, buf, size);
}

/*
 * Send a signal packet (S AA).
 */
static int gdb_send_signal_packet(struct gdb_state *state, char *buf,
                                  unsigned int buf_len, char signal)
{
    unsigned int size;
    int status;

    if (buf_len < 4) {
        /* Buffer too small */
        return GDB_EOF;
    }

    buf[0] = 'S';
    status = gdb_enc_hex(&buf[1], buf_len-1, &signal, 1);
    if (status == GDB_EOF) {
        return GDB_EOF;
    }
    size = 1 + status;
    return gdb_send_packet(state, buf, size);
}

/*
 * Send a error packet (E AA).
 */
static int gdb_send_error_packet(struct gdb_state *state, char *buf,
                                 unsigned int buf_len, char error)
{
    unsigned int size;
    int status;

    if (buf_len < 4) {
        /* Buffer too small */
        return GDB_EOF;
    }

    buf[0] = 'E';
    status = gdb_enc_hex(&buf[1], buf_len-1, &error, 1);
    if (status == GDB_EOF) {
        return GDB_EOF;
    }
    size = 1 + status;
    return gdb_send_packet(state, buf, size);
}
