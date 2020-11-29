#pragma once

#include <stdint.h>


struct cpuinfo_freebsd_topology {
	uint32_t packages;
	uint32_t cores;
	uint32_t threads;
};


struct cpuinfo_freebsd_topology cpuinfo_freebsd_detect_topology(void);
