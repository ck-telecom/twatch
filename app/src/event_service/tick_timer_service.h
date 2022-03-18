#pragma once

typedef enum TimeUnits {
	SECOND_UNIT,
	MINUTE_UNIT,
	HOUR_UNIT,
	DAY_UNIT,
	MONTH_UNIT,
	YEAR_UNIT	
} TimeUnits;

typedef void(* TickHandler)(struct tm *tick_time, TimeUnits units_changed);