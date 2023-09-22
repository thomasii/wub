

/*****************************************************************************
 * Packet Functions
 ****************************************************************************/

/*
 * Receive a packet acknowledgment
 *
 * Returns:
 *    0   if an ACK (+) was received
 *    1   if a NACK (-) was received
 *    GDB_EOF otherwise
 */
static int gdb_recv_ack(struct gdb_state *state)
{
    int response;

    /* Wait for packet ack */
    switch (response = gdb_sys_getc(state)) {
    case '+':
        /* Packet acknowledged */
        return 0;
    case '-':
        /* Packet negative acknowledged */
        return 1;
    default:
        /* Bad response! */
        GDB_PRINT("received bad packet response: 0x%2x\n", response);
        return GDB_EOF;
    }
}

/*
 * Calculate 8-bit checksum of a buffer.
 *
 * Returns:
 *    8-bit checksum.
 */
static int gdb_checksum(const char *buf, unsigned int len)
{
    unsigned char csum;

    csum = 0;

    while (len--) {
        csum += *buf++;
    }

    return csum;
}

/*
 * Transmits a packet of data.
 * Packets are of the form: $<packet-data>#<checksum>
 *
 * Returns:
 *    0   if the packet was transmitted and acknowledged
 *    1   if the packet was transmitted but not acknowledged
 *    GDB_EOF otherwise
 */
static int gdb_send_packet(struct gdb_state *state, const char *pkt_data,
                           unsigned int pkt_len)
{
    char buf[3];
    char csum;

    /* Send packet start */
    if (gdb_sys_putchar(state, '$') == GDB_EOF) {
        return GDB_EOF;
    }

#if DEBUG
    {
        unsigned int p;
        GDB_PRINT("-> ");
        for (p = 0; p < pkt_len; p++) {
            if (gdb_is_printable_char(pkt_data[p])) {
                GDB_PRINT("%c", pkt_data[p]);
            } else {
                GDB_PRINT("\\x%02x", pkt_data[p]&0xff);
            }
        }
        GDB_PRINT("\n");
    }
#endif

    /* Send packet data */
    if (gdb_write(state, pkt_data, pkt_len) == GDB_EOF) {
        return GDB_EOF;
    }

    /* Send the checksum */
    buf[0] = '#';
    csum = gdb_checksum(pkt_data, pkt_len);
    if ((gdb_enc_hex(buf+1, sizeof(buf)-1, &csum, 1) == GDB_EOF) ||
        (gdb_write(state, buf, sizeof(buf)) == GDB_EOF)) {
        return GDB_EOF;
    }

    return gdb_recv_ack(state);
}

/*
 * Receives a packet of data, assuming a 7-bit clean connection.
 *
 * Returns:
 *    0   if the packet was received
 *    GDB_EOF otherwise
 */
static int gdb_recv_packet(struct gdb_state *state, char *pkt_buf,
                           unsigned int pkt_buf_len, unsigned int *pkt_len)
{
    int data;
    char expected_csum, actual_csum;
    char buf[2];

    /* Wait for packet start */
    actual_csum = 0;

    while (1) {
        data = gdb_sys_getc(state);
        if (data == GDB_EOF) {
            return GDB_EOF;
        } else if (data == '$') {
            /* Detected start of packet. */
            break;
        }
    }

    /* Read until checksum */
    *pkt_len = 0;
    while (1) {
        data = gdb_sys_getc(state);

        if (data == GDB_EOF) {
            /* Error receiving character */
            return GDB_EOF;
        } else if (data == '#') {
            /* End of packet */
            break;
        } else {
            /* Check for space */
            if (*pkt_len >= pkt_buf_len) {
                GDB_PRINT("packet buffer overflow\n");
                return GDB_EOF;
            }

            /* Store character and update checksum */
            pkt_buf[(*pkt_len)++] = (char) data;
        }
    }

#if DEBUG
    {
        unsigned int p;
        GDB_PRINT("<- ");
        for (p = 0; p < *pkt_len; p++) {
            if (gdb_is_printable_char(pkt_buf[p])) {
                GDB_PRINT("%c", pkt_buf[p]);
            } else {
                GDB_PRINT("\\x%02x", pkt_buf[p] & 0xff);
            }
        }
        GDB_PRINT("\n");
    }
#endif

    /* Receive the checksum */
    if ((gdb_read(state, buf, sizeof(buf), 2) == GDB_EOF) ||
        (gdb_dec_hex(buf, 2, &expected_csum, 1) == GDB_EOF)) {
        return GDB_EOF;
    }

    /* Verify checksum */
    actual_csum = gdb_checksum(pkt_buf, *pkt_len);
    if (actual_csum != expected_csum) {
        /* Send packet nack */
        GDB_PRINT("received packet with bad checksum\n");
        gdb_sys_putchar(state, '-');
        return GDB_EOF;
    }

    /* Send packet ack */
    gdb_sys_putchar(state, '+');
    return 0;
}
