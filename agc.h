#ifndef __AGC_H__
#define __AGC_H__

#include <ncurses.h>

typedef struct agc agc_t;

agc_t *agc_new(WINDOW *scr);

int agc_del(agc_t *a);


int agc_key_handler(agc_t *a, int key);

int agc_draw_refresh(agc_t *a);




#endif  /* __AGC_H__ */
