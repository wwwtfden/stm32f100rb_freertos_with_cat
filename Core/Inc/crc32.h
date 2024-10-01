#ifndef __CRC32_H_
#define __CRC32_H_

#include "board.h"

uint32_t crc32(uint32_t crc, uint32_t *buffer, uint32_t len);