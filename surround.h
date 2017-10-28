#ifndef __SURROUND_H__
#define __SURROUND_H__

#include <ncurses.h>

typedef struct surround surround_t;

surround_t *surround_new(WINDOW *scr);

int surround_del(surround_t *a);


int surround_key_handler(surround_t *a, int key);

int surround_draw_refresh(surround_t *a);





















#endif  /* __SURROUND_H__ */
