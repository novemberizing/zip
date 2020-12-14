// http://www.iana.org/assignments/media-types/application/zip

#include <stdint.h>
#include <stdio.h>

#include "zip.h"

#define SIGNATURE_FILE          0x04034b50
#define SIGNATURE_CENTRAL       0x02014b50
#define SIGNATURE_CENTRAL_END   0x06054b50

typedef struct __signature
{
    uint32_t value;
} __attribute__((aligned(1), packed)) signature;

typedef struct __file
{
    signature signature;
    uint16_t version;
    uint16_t flags;
    uint16_t method;
    uint16_t time;
    uint16_t date;
    struct {
        uint32_t crc32;
        uint32_t compressed;
        uint32_t uncompressed;
    } __attribute__((aligned(1), packed)) data;
    uint16_t filename;
    uint16_t extra;
} __attribute__((aligned(1), packed)) fileTag;

typedef struct __central
{
    signature signature;
    uint16_t made;
    uint16_t version;
    uint16_t flags;
    uint16_t method;
    uint16_t time;
    uint16_t date;
    struct {
        uint32_t crc32;
        uint32_t compressed;
        uint32_t uncompressed;
    } __attribute__((aligned(1), packed)) data;
    uint16_t filename;
    uint16_t extra;
    uint16_t comment;
    uint16_t disk;
    uint16_t internal;
    uint32_t external;
    uint32_t relative;
} __attribute__((aligned(1), packed)) centralTag;

typedef struct __end
{
    signature signature;
    uint16_t disk;
    uint16_t start;
    uint16_t tracks;
    uint16_t directories;
    uint32_t size;
    uint32_t offset;
    uint16_t comment;
} __attribute__((aligned(1), packed)) endTag;



int unzip(const char * path)
{
    FILE * fp = fopen(path, "rb");
    if(fp) {
        fclose(fp);
    }
    return -1;
}

int zip(const char * path)
{
    return -1;
}