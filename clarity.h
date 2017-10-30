#ifndef __CLARITY_H__
#define __CLARITY_H__

#include <ncurses.h>

typedef struct clarity clarity_t;

clarity_t *clarity_new(WINDOW *scr);
int clarity_del(clarity_t *c);


int clarity_key_handler(clarity_t *c, int key);

int clarity_draw_refresh(clarity_t *c);























#endif  /* __CLARITY_H__ */
