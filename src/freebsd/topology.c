#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/sysctl.h>

#include <cpuinfo/log.h>
#include <freebsd/api.h>



struct cpuinfo_freebsd_topology cpuinfo_freebsd_detect_topology(void) {
	int cores = 1;
	size_t sizeof_cores = sizeof(cores);
	if (sysctlbyname("hw.ncpu", &cores, &sizeof_cores, NULL, 0) != 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.ncpu\") failed: %s", strerror(errno));
	} else if (cores <= 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.ncpu\") returned invalid value %d", cores);
		cores = 1;
	}

	int threads = 1;
	size_t sizeof_threads = sizeof(threads);
	if (sysctlbyname("hw.ncpu", &threads, &sizeof_threads, NULL, 0) != 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.ncpu\") failed: %s", strerror(errno));
	} else if (threads <= 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.ncpu\") returned invalid value %d", threads);
		threads = cores;
	}

	int packages = 1;
	size_t sizeof_packages = sizeof(packages);
	if (sysctlbyname("hw.packages", &packages, &sizeof_packages, NULL, 0) != 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.packages\") failed: %s", strerror(errno));
	} else if (packages <= 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.packages\") returned invalid value %d", packages);
		packages = 1;
	}

	cpuinfo_log_debug("freebsd topology: packages = %d, cores = %d, threads = %d", packages, (int) cores, (int) threads);
	struct cpuinfo_freebsd_topology topology = {
		.packages = (uint32_t) packages,
		.cores = (uint32_t) cores,
		.threads = (uint32_t) threads
	};

	size_t cacheconfig_size = 0;
	if (sysctlbyname("hw.cacheconfig", NULL, &cacheconfig_size, NULL, 0) != 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.cacheconfig\") failed: %s", strerror(errno));
	} else {
		uint64_t* cacheconfig = malloc(cacheconfig_size);
		if (sysctlbyname("hw.cacheconfig", cacheconfig, &cacheconfig_size, NULL, 0) != 0) {
			cpuinfo_log_error("sysctlbyname(\"hw.cacheconfig\") failed: %s", strerror(errno));
		} else {
			size_t cache_configs = cacheconfig_size / sizeof(uint64_t);
			cpuinfo_log_debug("freebsd hw.cacheconfig count: %zu", cache_configs);
			if (cache_configs > CPUINFO_FREEBSD_MAX_CACHE_LEVELS) {
				cache_configs = CPUINFO_FREEBSD_MAX_CACHE_LEVELS;
			}
			for (size_t i = 0; i < cache_configs; i++) {
				cpuinfo_log_debug("freebsd hw.cacheconfig[%zu]: %"PRIu64, i, cacheconfig[i]);
				topology.threads_per_cache[i] = cacheconfig[i];
			}
		}
	}
	return topology;
}
