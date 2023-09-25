/*****************************************************************************
 * Packet Creation Helpers
 ****************************************************************************/

/**
 * @brief Send an 'OK' packet to the debugging console.
 *
 * @param state The gdb_state structure containing debugging state information.
 * @param buf The buffer used to store packet data.
 * @param buf_len The length of the buffer.
 * @return Status of the packet sending operation.
 */
static int gdb_send_ok_packet(struct gdb_state *state, char *buf,
                              unsigned int buf_len)
{
    return gdb_send_packet(state, "OK", 2);
}

/**
 * @brief Send a message to the debugging console using the 'O XX...' packet format.
 *
 * @param state The gdb_state structure containing debugging state information.
 * @param buf The buffer used to store packet data.
 * @param buf_len The length of the buffer.
 * @param msg The message to be sent.
 * @return Status of the packet sending operation.
 */
static int gdb_send_conmsg_packet(struct gdb_state *state, char *buf,
                                  unsigned int buf_len, const char *msg)
{
    unsigned int size;
    int status;

    // ... (your code here)

    return gdb_send_packet(state, buf, size);
}

/**
 * @brief Send a signal packet using the 'S AA' format.
 *
 * @param state The gdb_state structure containing debugging state information.
 * @param buf The buffer used to store packet data.
 * @param buf_len The length of the buffer.
 * @param signal The signal code.
 * @return Status of the packet sending operation.
 */
static int gdb_send_signal_packet(struct gdb_state *state, char *buf,
                                  unsigned int buf_len, char signal)
{
    unsigned int size;
    int status;

    // ... (your code here)

    return gdb_send_packet(state, buf, size);
}

/**
 * @brief Send an error packet using the 'E AA' format.
 *
 * @param state The gdb_state structure containing debugging state information.
 * @param buf The buffer used to store packet data.
 * @param buf_len The length of the buffer.
 * @param error The error code.
 * @return Status of the packet sending operation.
 */
static int gdb_send_error_packet(struct gdb_state *state, char *buf,
                                 unsigned int buf_len, char error)
{
    unsigned int size;
    int status;

    // ... (your code here)

    return gdb_send_packet(state, buf, size);
}
