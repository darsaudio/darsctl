#ifndef __BASS_H__
#define __BASS_H__

#include <ncurses.h>

typedef struct bass bass_t;

bass_t *bass_new(WINDOW *scr);
int bass_del(bass_t *bs);


int bass_key_handler(bass_t *bs, int key);

int bass_draw_refresh(bass_t *bs);














#endif      /* __BASS_H__ */
