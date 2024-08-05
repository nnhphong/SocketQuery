#ifndef _RECORD_H
#define _RECORD_H

#define NAME_LEN_MAX 29

typedef struct record {
  unsigned char name_len;
  char name[NAME_LEN_MAX];
  unsigned short sunspots;
} record;

#endif  // #ifndef _RECORD_H
