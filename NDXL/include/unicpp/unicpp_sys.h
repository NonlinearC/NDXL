//----------------------------------\\
// C/C++ cross-platform functions	\\
// Ethan Codron 2025		        \\
// GNU GPL-3.0				        \\
//----------------------------------\\
// Header file abstracting platform	\\
// specific calls like querying the	\\
// clock time or memory allocation.	\\
//----------------------------------\\

#ifndef _UNICPP_SYS_
#define _UNICPP_SYS_

#include "unicpp_compiler.h"
#include "unicpp_os.h"

// System time vvvvv

#if UNI_COMPILER == UNI_COMPILER_MSVC
	static inline const size_t uniTimeNs()
	{
		static struct timespec ts;
		timespec_get(&ts, TIME_UTC);
		return ts.tv_nsec;
	}

	static inline const size_t uniTimeSec()
	{
		static struct timespec ts;
		timespec_get(&ts, TIME_UTC);
		return ts.tv_sec;
	}
#else
	static inline const size_t uniTimeNs()
	{
		static struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return ts.tv_nsec;
	}

	static inline const size_t uniTimeSec()
	{
		static struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return ts.tv_sec;
	}
#endif

// ^^^^^ System time | llocation tracing vvvvv

#if (UNI_PLATFORM == UNI_PLATFORM_WIN64) || (UNI_PLATFORM == UNI_PLATFORM_WIN32)
	#include <psapi.h>

	static inline const double uniPeakMemoryUsage()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.PeakWorkingSetSize / 1024.0;
	}

	static inline const double uniCurrentMemoryUsage()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.WorkingSetSize / 1024.0;
	}
#else
	#include <sys/resource.h>

	static inline const double uniPeakMemoryUsage()
	{
		struct rusage usage;
		getrusage(RUSAGE_SELF, &usage);
		return (double)usage.ru_maxrss;
	}
	#if (UNI_PLATFORM == UNI_PLATFORM_LINUX64) || (UNI_PLATFORM == UNI_PLATFORM_LINUX32)
		#include <stdio.h>
		#include <unistd.h>

		static inline const double uniCurrentMemoryUsage()
		{
			long rss_pages = 0;
			FILE *fp = fopen("/proc/self/statm", "r");
			if (fp) {
				if (fscanf(fp, "%*s %ld", &rss_pages) != 1)
					rss_pages = 0;
				fclose(fp);
			}
			return rss_pages * sysconf(_SC_PAGESIZE) / 1024.0;
		}
	#elif UNI_PLATFORM == UNI_PLATFORM_MACOS
		#include <mach/mach.h>

		static inline const double uniCurrentMemoryUsage()
		{
			struct mach_task_basic_info info;
			mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
			task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count);
			return info.resident_size / 1024.0;
		}
	#endif
#endif // Windows

// ^^^^^ Allocation tracing

#endif // _UNICPP_SYS_