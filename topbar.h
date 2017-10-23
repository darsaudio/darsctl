#ifndef __TOP_BAR_H__
#define __TOP_BAR_H__
#include <ncurses.h>

typedef struct topbar topbar_t;

topbar_t *topbar_new(WINDOW *scr);
int topbar_del(topbar_t *tb);

int topbar_set_active_index(topbar_t *tb, int i);
int topbar_draw_refresh(topbar_t *tb);








#endif      /* __TOP_BAR_H__ */
