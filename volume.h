#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>


typedef struct volume volume_t;

volume_t *volume_new(WINDOW *scr);
int volume_del(volume_t *vm);

int volume_key_handler(volume_t *vm, int key);

int volume_draw_refresh(volume_t *vm);

















#endif  /* __VOLUME_H__ */
