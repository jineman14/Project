#ifndef CRC64_H
#define CRC64_H


unsigned short crc16(unsigned short crc, const void *buf, size_t size);

unsigned int crc32(unsigned int crc, const void *buf, size_t size);

unsigned __int64 crc64(unsigned __int64 crc, const unsigned char *s, unsigned __int64 l);

#endif
