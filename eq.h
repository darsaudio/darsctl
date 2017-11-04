#ifndef __EQ_H__
#define __EQ_H__

#include <ncurses.h>

typedef struct eq eq_t;

eq_t *eq_new(WINDOW *scr);
int eq_del(eq_t *p);


int eq_key_handler(eq_t *p, int key);

int eq_draw_refresh(eq_t *p);
























#endif  /* __EQ_H__ */
