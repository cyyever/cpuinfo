#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/sysctl.h>

#include <cpuinfo/log.h>
#include <freebsd/api.h>



struct cpuinfo_freebsd_topology cpuinfo_freebsd_detect_topology(void) {
	int threads = 1;
	size_t sizeof_threads = sizeof(threads);
	if (sysctlbyname("hw.ncpu", &threads, &sizeof_threads, NULL, 0) != 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.ncpu\") failed: %s", strerror(errno));
	} else if (threads <= 0) {
		cpuinfo_log_error("sysctlbyname(\"hw.ncpu\") returned invalid value %d", threads);
	}

    int cores = threads / 2;
	int packages = 1;

	cpuinfo_log_debug("freebsd topology: packages = %d, cores = %d, threads = %d", packages, (int) cores, (int) threads);
	struct cpuinfo_freebsd_topology topology = {
		.packages = (uint32_t) packages,
		.cores = (uint32_t) cores,
		.threads = (uint32_t) threads
	};

	return topology;
}
