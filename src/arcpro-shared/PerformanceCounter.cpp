/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (c) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (c) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "PerformanceCounter.hpp"
#include "SysInfo.hpp"

namespace Arcpro
{

	PerformanceCounter::PerformanceCounter()
	{
		cpu_count = Arcpro::SysInfo::GetCPUCount();
		last_update = Arcpro::SysInfo::GetTickCount();
		last_cpu_usage = Arcpro::SysInfo::GetCPUUsage();
	}

	float PerformanceCounter::GetCurrentRAMUsage()
	{
		unsigned long long usage = Arcpro::SysInfo::GetRAMUsage();

		return static_cast< float >(usage / (1024.0 * 1024.0));
	}

	float PerformanceCounter::GetCurrentCPUUsage()
	{
		unsigned long long now = Arcpro::SysInfo::GetTickCount();
		unsigned long long now_cpu_usage = Arcpro::SysInfo::GetCPUUsage();
		unsigned long long cpu_usage = now_cpu_usage - last_cpu_usage; // micro seconds
		unsigned long long time_elapsed = now - last_update; // milli seconds

		// converting to micro seconds
		time_elapsed *= 1000;

		float cpu_usage_percent = static_cast< float >(double(cpu_usage) / double(time_elapsed));

		cpu_usage_percent *= 100;

		// If we have more than 1 CPUs/cores,
		// without dividing here we could get over 100%
		cpu_usage_percent /= cpu_count;

		last_update = now;
		last_cpu_usage = now_cpu_usage;

		return static_cast< float >(cpu_usage_percent);
	}
}
