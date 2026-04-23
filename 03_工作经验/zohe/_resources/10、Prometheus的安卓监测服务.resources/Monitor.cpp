#include "Monitor.h"
#include <sys/statvfs.h>
#include <dirent.h>
#include <unistd.h>

struct CpuTimes 
{
	unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
};

CpuTimes getSysCpuTimes() 
{
	std::string line;
	CpuTimes times;
	memset(&times, 0, sizeof(CpuTimes));

	std::ifstream file("/proc/stat");
	if (file.is_open()) 
	{
		std::getline(file, line);
		std::istringstream ss(line);
		std::string cpuLabel;
		ss >> cpuLabel; // Skip the "cpu" label
		ss >> times.user >> times.nice >> times.system >> times.idle
			>> times.iowait >> times.irq >> times.softirq >> times.steal;
	}
	else
	{
		printf("Unable to open /proc/stat\n");
	}

	file.close();

	return times;
}

double getProcessCpuTimes(int pid)
{
	std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
	if (!statFile.is_open())
	{
		printf("can not open /proc/%d/stat\n", pid);
		return -1;
	}

	std::string line;
	getline(statFile, line);
	statFile.close();

	std::istringstream iss(line);
	std::vector<std::string> values((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

	if (values.size() < 22)
	{
		printf("values.size() < 22\n");
		return -1;
	}

	long utime = std::stol(values[13]);
	long stime = std::stol(values[14]);
	long cutime = std::stol(values[15]);
	long cstime = std::stol(values[16]);
	long total_time = utime + stime + cutime + cstime;

	return total_time;  // 返回进程总 CPU 时间（用户态+内核态）
}

unsigned long long getTotalTime(const CpuTimes& times) 
{
	return times.user + times.nice + times.system + times.idle +
		times.iowait + times.irq + times.softirq + times.steal;
}

unsigned long long getUsageTime(const CpuTimes& times)
{
	return times.user + times.nice + times.system + 
		times.irq + times.softirq + times.steal;
}

void GetCpuUsage(prometheus::Gauge& sysCpuUsg, prometheus::Gauge& procCpuUsg, int pid)
{
	CpuTimes firstRead = getSysCpuTimes();
	double firstProcTimes = getProcessCpuTimes(pid);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	CpuTimes secondRead = getSysCpuTimes();
	double secondProcTimes = getProcessCpuTimes(pid);

	unsigned long long totalFirst = getTotalTime(firstRead);
	unsigned long long totalSecond = getTotalTime(secondRead);

	unsigned long long totalUsageFirst = getUsageTime(firstRead);
	unsigned long long totalUsageSecond = getUsageTime(secondRead);

	double totalDiff = totalSecond - totalFirst;
	double usageDiff = totalUsageSecond - totalUsageFirst;
	double procDiff = secondProcTimes - firstProcTimes;

	double cpuUsage = (usageDiff / totalDiff) * 100;
	double procUsage = (procDiff / totalDiff) * 100;
	
	sysCpuUsg.Set(cpuUsage);
	procCpuUsg.Set(procUsage);
}

void GetSysMemUsage(prometheus::Gauge& memTotal, prometheus::Gauge& memAvaliable)
{
	std::ifstream file("/proc/meminfo");
	if (file.is_open()) 
	{
		std::string line;
		double total = 0.0;
		double available = 0.0;
		while (getline(file, line)) 
		{
			if (line.find("MemTotal") != std::string::npos)
			{
				sscanf(line.c_str(), "MemTotal: %lf kB", &total);
				memTotal.Set(total);
			}
			else if (line.find("MemAvailable") != std::string::npos)
			{
				sscanf(line.c_str(), "MemAvailable: %lf kB", &available);
				memAvaliable.Set(available);
			}
		}
	}
	else 
	{
		printf("Unable to open /proc/meminfo\n");
	}

	file.close();
}

void GetSysStorageUsage(std::string path, prometheus::Gauge& storageTotal, prometheus::Gauge& storageFree)
{
	struct statvfs stat;
	if (statvfs(path.c_str(), &stat) == 0) 
	{
		unsigned long totalSpace = stat.f_blocks * stat.f_frsize;
		unsigned long freeSpace = stat.f_bfree * stat.f_frsize;
		double totalSp = totalSpace / 1024;
		double freeSp = freeSpace / 1024;
		storageTotal.Set(totalSp);
		storageFree.Set(freeSp);
	}
	else 
	{
		printf("Failed to get storage information\n");
	}
}

void GetSysUpTime(prometheus::Gauge& time)
{
	std::ifstream file("/proc/uptime");
	if (file.is_open()) 
	{
		int uptime = 0;
		file >> uptime;
		time.Set(uptime);
	}
	else 
	{
		printf("Unable to open /proc/uptime\n");
	}

	file.close();
}

void GetProcessPid(const std::string processName, int& procPid)
{
	DIR* procDir = opendir("/proc");
	if (!procDir) 
	{
		printf("opendir /proc failed!!!\n");
		return;
	}

	struct dirent* entry;
	while ((entry = readdir(procDir)) != nullptr) 
	{
		if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) 
		{
			std::string pid = entry->d_name;
			std::string cmdPath = "/proc/" + pid + "/cmdline";
			std::ifstream cmdFile(cmdPath);
			if (cmdFile.is_open()) 
			{
				std::string cmdline;
				getline(cmdFile, cmdline);
				if (cmdline.find(processName) != std::string::npos) 
				{
					closedir(procDir);
					printf("pid:%s\n", pid.c_str());
					procPid = stoi(pid);
					return;
				}
			}
		}
	}
	closedir(procDir);
}

void GetProcMemUsage(prometheus::Gauge& mem, int pid)
{
	std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
	if (!statusFile.is_open()) 
	{
		printf("can not open /proc/%d/status\n", pid);
		return;
	}

	std::string line;
	while (getline(statusFile, line)) 
	{
		if (line.find("VmRSS:") == 0) 
		{
			std::istringstream iss(line);
			std::string key;
			long memory;
			iss >> key >> memory;
			mem.Set(memory);
		}
	}

	statusFile.close();
}

void GetProcThreadCount(prometheus::Gauge& threads, int pid)
{
	std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
	if (!statusFile.is_open()) 
	{
		printf("can not open /proc/%d/status\n", pid);
		return;
	}

	std::string line;
	while (getline(statusFile, line)) 
	{
		if (line.find("Threads:") == 0) 
		{
			std::istringstream iss(line);
			std::string key;
			int threadCount;
			iss >> key >> threadCount;
			threads.Set(threadCount);
		}
	}
	
	statusFile.close();
}

void GetProcUptime(prometheus::Gauge& uptime, int pid)
{
	std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
	if (!statFile.is_open()) 
	{
		printf("can not open /proc/%d/stat\n", pid);
		return;
	}

	std::string line;
	getline(statFile, line);
	statFile.close();

	std::istringstream iss(line);
	std::vector<std::string> values((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

	if (values.size() < 22) 
	{
		return;
	}

	long starttime = std::stol(values[21]);
	double systemUptime;
	std::ifstream uptimeFile("/proc/uptime");
	if (uptimeFile.is_open()) 
	{
		uptimeFile >> systemUptime;
		uptimeFile.close();
	}

	long clockTicks = sysconf(_SC_CLK_TCK);
	int processUptime = systemUptime - (starttime / clockTicks);

	uptime.Set(processUptime);
}

void GetProcNetworkUsage(prometheus::Gauge& network, int pid)
{
	std::ifstream netFile("/proc/" + std::to_string(pid) + "/net/dev");
	if (!netFile.is_open()) 
	{
		printf("can not open /proc/%d/net/dev\n", pid);
		return;
	}

	std::string line;
	long totalBytes = 0;
	while (getline(netFile, line)) 
	{
		if (line.find("eth0") != std::string::npos) 
		{
			// 根据实际使用的网络接口修改
			std::istringstream iss(line);
			std::string iface;
			long rxBytes, txBytes;
			iss >> iface >> rxBytes;
			for (int i = 0; i < 7; ++i) iss >> txBytes;
			totalBytes = rxBytes + txBytes;
			break;
		}
	}
	network.Set(totalBytes);  // 返回进出流量总和 (字节)
}