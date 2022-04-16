#ifndef _APP_H
#define _APP_H

#include <stdbool.h>

typedef struct _app app_t;

typedef struct app_spec {
	const char *name;
	int (*init)(app_t *app, lv_obj_t *parent);
	int (*event)(app_t *app, uint32_t event, unsigned long data);
	int (*exit)(app_t *app);
} app_spec_t;

struct _app {
	const app_spec_t *spec;
	bool dirty;
};

enum apps {
	CLOCK,
};

struct pinetimecos {
	app_t *running_app;
	enum apps return_app;
};

void load_application(enum apps app/*, enum RefreshDirections dir*/);

#endif /* _APP_H */