#ifndef _TICK_TIMER_SERVICE_H
#define _TICK_TIMER_SERVICE_H

#include <time.h>

typedef enum TimeUnits {
	SECOND_UNIT = BIT(0),
	MINUTE_UNIT = BIT(1),
	HOUR_UNIT = BIT(2),
	DAY_UNIT = BIT(3),
	MONTH_UNIT = BIT(4),
	YEAR_UNIT = BIT(5)
} TimeUnits;

struct tick_timer_service_data {
	struct tm tick_time;
	TimeUnits units_changed;
};

typedef void (* TickHandler)(struct tm *tick_time, TimeUnits units_changed);

void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler);

void tick_timer_service_unsubscribe(void);

#endif /* _TICK_TIMER_SERVICE_H */