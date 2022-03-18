

enum HealthMetric {
	HealthMetricStepCount,
	HealthMetricActiveSeconds,
	HealthMetricWalkedDistanceMeters,
	HealthMetricSleepSeconds,
	HealthMetricSleepRestfulSeconds,
	HealthMetricRestingKCalories,
	HealthMetricActiveKCalories,
	HealthMetricHeartRateBPM,
	HealthMetricHeartRateRawBPM
};

enum HealthActivity {
	HealthActivityNone,
	HealthActivitySleep,
	HealthActivityRestfulSleep,
	HealthActivityWalk,
	HealthActivityRun,
	HealthActivityOpenWorkout	
};