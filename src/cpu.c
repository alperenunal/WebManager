#include "cpu.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <sensors/sensors.h>

#define BUFFER_SIZE 1024

int getUptime(long *uptime) {
	FILE *file;
	if ((file = fopen("/proc/uptime", "r")) == NULL) {
		perror("fopen");
		return 0;
	}

	if (fscanf(file, "%ld", uptime) == EOF) {
		perror("fscanf");
		fclose(file);
		return 0;
	}

	fclose(file);

	return 1;
}

int getAvgLoad(double *load1, double *load5, double *load15) {
	FILE *file;
	if ((file = fopen("/proc/loadavg", "r")) == NULL) {
		perror("fopen");
		return 0;
	}

	if (fscanf(file, "%lf %lf %lf", load1, load5, load15) == EOF) {
		perror("fscanf");
		fclose(file);
		return 0;
	}

	fclose(file);

	return 1;
}

int getCPUUsage(int ncpu, double *usage) {
	FILE *file;
	uint64_t cpuid;
	uint64_t prev[ncpu][10], curr[ncpu][10];
	uint64_t prevIdle, currIdle;
	uint64_t prevTotal, currTotal;

	if ((file = fopen("/proc/stat", "r")) == NULL) {
		perror("fopen");
		return 0;
	}

	for (int i = -1; i < ncpu; ++i) {
		char buffer[BUFFER_SIZE] = {0};

		if (i == -1) {
			if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
				fclose(file);
				return 0;
			}
		} else {
			if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
				fclose(file);
				return 0;
			}
			sscanf(buffer, "cpu%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
				   &cpuid, &prev[i][0], &prev[i][1], &prev[i][2], &prev[i][3],
				   &prev[i][4], &prev[i][5], &prev[i][6], &prev[i][7],
				   &prev[i][8], &prev[i][9]);
		}
	}

	fseek(file, 0, SEEK_SET);
	sleep(1);

	for (int i = -1; i < ncpu; ++i) {
		char buffer[BUFFER_SIZE] = {0};

		if (i == -1) {
			if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
				fclose(file);
				return 0;
			}
		} else {
			if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
				fclose(file);
				return 0;
			}
			sscanf(buffer, "cpu%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
				   &cpuid, &curr[i][0], &curr[i][1], &curr[i][2], &curr[i][3],
				   &curr[i][4], &curr[i][5], &curr[i][6], &curr[i][7],
				   &curr[i][8], &curr[i][9]);
		}
	}

	for (int i = 0; i < ncpu; ++i) {
		prevIdle = prev[i][3];
		currIdle = curr[i][3];

		prevTotal = 0;
		currTotal = 0;

		for (int j = 0; j < 10; ++j) {
			prevTotal += prev[i][j];
			currTotal += curr[i][j];
		}

		uint64_t idleDiff = currIdle - prevIdle;
		uint64_t totalDiff = currTotal - prevTotal;

		usage[i] = (double)(totalDiff - idleDiff) / totalDiff * 100;
	}

	fclose(file);
	return 1;
}

double getTemp(void) {
	if (sensors_init(NULL)) {
		return -1;
	}

	double temp = 0.0;
	int n = 0;
	const sensors_chip_name *chip;

	while ((chip = sensors_get_detected_chips(NULL, &n))) {
		if (!strcmp("coretemp", chip->prefix)) {
			int x = 0;
			const sensors_feature *feature = sensors_get_features(chip, &x);

			const sensors_subfeature *sub = sensors_get_subfeature(
				chip, feature, SENSORS_SUBFEATURE_TEMP_INPUT);

			sensors_get_value(chip, sub->number, &temp);
			break;
		}
	}

	sensors_cleanup();

	return temp;
}
