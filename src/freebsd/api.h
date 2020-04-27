#pragma once

#include <stdint.h>

#define CPUINFO_FREEBSD_MAX_CACHE_LEVELS 8


struct cpuinfo_freebsd_topology {
	uint32_t packages;
	uint32_t cores;
	uint32_t threads;
	uint32_t threads_per_cache[CPUINFO_FREEBSD_MAX_CACHE_LEVELS];
};


struct cpuinfo_freebsd_topology cpuinfo_freebsd_detect_topology(void);
