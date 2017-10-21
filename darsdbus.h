#ifndef __DARSDBUS_H__
#define __DARSDBUS_H__

typedef struct darsdbus darsdbus_t;

darsdbus_t *darsdbus_new();
int darsdbus_del(darsdbus_t *bus);


/* caller should free return string */
char *darsdbus_get_property(darsdbus_t *bus, const char *name);


/* caller should free return string */
char *darsdbus_get_param(darsdbus_t *bus, const char *name);


int darsdbus_set_param(darsdbus_t *bus, const char *param, const char *value);








#endif      /* __DARSDBUS_H__ */
