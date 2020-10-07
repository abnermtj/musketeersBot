#pragma once

typedef enum {
    MESSAGE_N = 0x01,
    MESSAGE_NE = 0x02,
    MESSAGE_E = 0x03,
    MESSAGE_SE = 0x04,
    MESSAGE_S = 0x05,
    MESSAGE_SW = 0x06,
    MESSAGE_W = 0x07,
    MESSAGE_NW = 0x08,
    MESSAGE_STOP = 0x00,
    MESSAGE_BT_CONNECT = 0x0A,
    MESSAGE_EMPTY = 0x50
}message_list_t;