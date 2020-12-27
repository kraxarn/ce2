#pragma once

/*
 * Based off code by David Robert Nadaeu
 * http://nadeausoftware.com/articles/2012/07/c_c_tip_how_get_process_resident_set_size_physical_memory_use
 */

// Windows
#include <windows.h>
#include <Psapi.h>

// Check CrowMemory.h for usage in other operating systems

namespace ce
{
	class Memory
	{
	public:
		enum MemoryUnit { bytes, kilobytes, megabytes };

		static float getPeak(const MemoryUnit unit)
		{
			// Windows
			PROCESS_MEMORY_COUNTERS info;
			GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof info);
			return toMemoryUnit(info.PeakWorkingSetSize, unit);
		}

		static float getUsage(const MemoryUnit unit)
		{
			// Windows
			PROCESS_MEMORY_COUNTERS info;
			GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof info);
			return toMemoryUnit(info.WorkingSetSize, unit);
		}

		static float getPageUsage(const MemoryUnit unit)
		{
			// Windows
			PROCESS_MEMORY_COUNTERS info;
			GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof info);
			return toMemoryUnit(info.PagefileUsage, unit);
		}

	private:
		// Helper to convert to desired unit
		static float toMemoryUnit(const float memory, const MemoryUnit unit)
		{
			switch (unit)
			{
				case bytes:     return memory;
				case kilobytes: return memory / 1000.f;
				case megabytes: return memory / 1000000.f;
				default: return 0;
			}
		}
	};
}