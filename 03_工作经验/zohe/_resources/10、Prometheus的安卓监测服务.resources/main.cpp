#include "Monitor.h"

#define	MONITOR_INTERVAL	2
#define MONITOR_PROCESS		"Monitor"
#define	STORAGE_PATH		"/data"

int main()
{
	prometheus::Exposer exposer{ "0.0.0.0:8080" };

	// create a metrics registry
	auto registry = std::make_shared<prometheus::Registry>();
	auto& procRunningGauge = prometheus::BuildGauge().Name("process_running").Help("Process is Running").Register(*registry).Add({ {"type", "process"}, {"name", MONITOR_PROCESS} });
	auto& CpuGauge = prometheus::BuildGauge().Name("cpu_usage").Help("Current CPU usage").Register(*registry);
	auto& memGauge = prometheus::BuildGauge().Name("mem_usage").Help("Current MEM usage").Register(*registry);
	auto& storageGauge = prometheus::BuildGauge().Name("sys_storage_usage").Help("System Storage usage").Register(*registry);
	auto& uptimeGauge = prometheus::BuildGauge().Name("uptime").Help("System and Process Uptime").Register(*registry);
	auto& threadsGauge = prometheus::BuildGauge().Name("thread_count").Help("Process Thread Count").Register(*registry).Add({ {"type", "process"}, {"name", MONITOR_PROCESS} });
	auto& procNetGauge = prometheus::BuildGauge().Name("process_net_usage").Help("Process Network Usage").Register(*registry).Add({ {"type", "process"}, {"name", MONITOR_PROCESS}, {"unit", "bytes"} });

	auto& sysCpuUsage = CpuGauge.Add({ {"type", "system"}, {"unit", "percentage"} });
	auto& procCpuUsage = CpuGauge.Add({ {"type", MONITOR_PROCESS}, {"unit", "percentage"} });
	auto& memTotal = memGauge.Add({ { "type", "total" }, {"unit", "KB"} });
	auto& memAvailable = memGauge.Add({ { "type", "available" }, {"unit", "KB"} });
	auto& memProc = memGauge.Add({ { "type", MONITOR_PROCESS }, {"unit", "KB"} });
	auto& storageTotal = storageGauge.Add({ { "type", "total" }, {"unit", "KB"} });
	auto& storageFree = storageGauge.Add({ { "type", "free" }, {"unit", "KB"} });
	auto& uptimeSys = uptimeGauge.Add({ { "type", "system" }, {"unit", "seconds"} });
	auto& uptimeProc = uptimeGauge.Add({ { "type", MONITOR_PROCESS }, {"unit", "seconds"} });

	exposer.RegisterCollectable(registry);

	while (true)
	{
		int procPid = -1;
		GetProcessPid(MONITOR_PROCESS, procPid);
		GetCpuUsage(sysCpuUsage, procCpuUsage, procPid);
		GetSysMemUsage(memTotal, memAvailable);
		GetSysStorageUsage(STORAGE_PATH, storageTotal, storageFree);
		GetSysUpTime(uptimeSys);

		GetProcMemUsage(memProc, procPid);
		GetProcThreadCount(threadsGauge, procPid);
		GetProcUptime(uptimeProc, procPid);
		GetProcNetworkUsage(procNetGauge, procPid);

		procRunningGauge.Set(procPid == -1 ? 0 : 1);

		std::this_thread::sleep_for(std::chrono::seconds(MONITOR_INTERVAL));
	}

	return 0;
}