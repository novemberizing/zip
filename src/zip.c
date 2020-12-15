// http://www.iana.org/assignments/media-types/application/zip

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "zip.h"

#define SIGNATURE_FILE          0x04034b50
#define SIGNATURE_CENTRAL       0x02014b50
#define SIGNATURE_CENTRAL_END   0x06054b50

typedef struct __File
{
    uint32_t signature;
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
} __attribute__((aligned(1), packed)) File;

typedef struct __Central
{
    uint32_t signature;
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
} __attribute__((aligned(1), packed)) Central;

typedef struct __End
{
    uint32_t signature;
    uint16_t disk;
    uint16_t start;
    uint16_t tracks;
    uint16_t directories;
    uint32_t size;
    uint32_t offset;
    uint16_t comment;
} __attribute__((aligned(1), packed)) End;

typedef struct Item
{
    struct {
        File * data;
        char * name;
        char * extra;
    } file;
    struct {
        Central * data;
        char * name;
        char * extra;
        char * comment;
    } info;
    struct Item * prev;
    struct Item * next;
} Item;

typedef struct __Root {
    struct {
        End * data;
        char * comment;
    } info;
    Item * head;
    Item * tail;
    uint32_t len;
} Root;

static Item * __generate_item(Root * root);
static Item * __generate_file_tag(Item * item, FILE * fp);

static Item * __generate_file_tag(Item * item, FILE * fp)
{
    if(item && fp) {
        if(item->file.data) {
            free(item->file.data);
            item->file.data = NULL;
        }
        item->file.data = (File *) calloc(sizeof(File), 1);
        if(item->file.data != NULL) {
            if(fread(item->file.data, sizeof(unsigned char), sizeof(File), fp) == sizeof(File)) {
                // FILE NAME RETRIEVE
                if(item->file.name) {
                    free(item->file.name);
                    item->file.name = NULL;
                }
                item->file.name = (char *) malloc(item->file.data->filename + 1);
                if(item->file.name == NULL) {
                    free(item->file.data);
                    item->file.data = NULL;
                    fprintf(stdout, "fail to __generate_file_tag caused by malloc (%d)\n", errno);
                    return NULL;
                }
                if(fread(item->file.name, sizeof(unsigned char), item->file.data->filename, fp) != item->file.data->filename) {
                    free(item->file.name);
                    item->file.name = NULL;
                    free(item->file.data);
                    item->file.data = NULL;
                    fprintf(stdout, "fail to __generate_file_tag caused by fread\n");
                    return NULL;
                }
                item->file.name[item->file.data->filename] = 0;
                // EXTRA RETRIEVE
                if(item->file.extra) {
                    free(item->file.extra);
                    item->file.extra = NULL;
                }
                if(item->file.data->extra > 0) {
                    item->file.extra = (unsigned char *) malloc(sizeof(item->file.data->extra));
                    if(item->file.extra == NULL) {
                        free(item->file.name);
                        item->file.name = NULL;
                        free(item->file.data);
                        item->file.data = NULL;
                        fprintf(stdout, "fail to __generate_file_tag caused by malloc (%d)\n", errno);
                        return NULL;
                    }
                    if(fread(item->file.extra, sizeof(unsigned char), item->file.data->extra, fp) != item->file.data->extra) {
                        free(item->file.name);
                        item->file.name = NULL;
                        free(item->file.extra);
                        item->file.extra = NULL;
                        free(item->file.data);
                        item->file.data = NULL;
                        fprintf(stdout, "fail to __generate_file_tag caused by fread\n");
                        return NULL;
                    }
                }
                return item;
            }
            // TODO: REASON TO FAIL
            fprintf(stdout, "fail to __generate_file_tag caused by fread\n");
            return NULL;
        }
        fprintf(stdout, "fail to __generate_file_tag caused by calloc (%d)\n", errno);
        return NULL;
    }
    fprintf(stdout, "fail to __generate_file_tag caused by item or fp is nullptr\n");
    return NULL;
}

static Item * __generate_item(Root * root)
{
    if(root) {
        Item * item = (Item *) calloc(sizeof(Item), 1);
        if(item) {
            item->prev = root->tail;
            root->tail->next = item;
            if(root->head == NULL) {
                root->head = item;
            }
            root->tail = item;
            root->len++;
            return item;
        }
        fprintf(stdout, "fail to generate item caused by malloc (%d)\n", errno);    
        return NULL;
    }
    fprintf(stdout, "fail to generate item caused by root is nullptr\n");
    return NULL;
}

static uint32_t __get_signature(FILE * fp);

int unzip(const char * path)
{
    FILE * fp = fopen(path, "rb");
    if(fp) {
        Root root = { 0, };
        uint32_t signature = 0;
        while((signature = __get_signature(fp)) != 0) {
            if(signature == SIGNATURE_FILE) {
                Item * item = __generate_item(&root);
                if(__generate_file_tag(item, fp) == NULL) {
                    fprintf(stdout, "fail to __generate_file_tag\n");
                    break;
                }
            } else if(signature == SIGNATURE_CENTRAL) {
                break;
            } else if(signature == SIGNATURE_CENTRAL_END) {
                break;
            } else {
                fprintf(stdout, "fail to unzip caused by unknown error\n");
            }
        }
        fclose(fp);
    }
    return -1;
}

int zip(const char * path)
{
    return -1;
}

static uint32_t __get_signature(FILE * fp)
{
    uint32_t v = 0;
    if(fread(&v, sizeof(uint8_t), sizeof(uint32_t), fp) == sizeof(uint32_t)) {
        if(fseek(fp, -4, SEEK_CUR) < 0) {
            fprintf(stdout, "fail to fseek caused by %d\n", errno);
            return 0;
        }
        return v;
    }
    // TODO: CHECK FAIL REASON
    fprintf(stdout, "fail to get signature\n");
    return 0;
}