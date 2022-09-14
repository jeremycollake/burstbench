// BurstBench - (c)2022 Jeremy Collake - https://bitsum.com
// See LICENSE - GPLv3
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <chrono>
#include <vector>
#include <conio.h>
#include <boost/program_options.hpp>
#include <boost/uuid/random_generator.hpp>
#include "../libs/md5/md5wrapper.h"

namespace po = boost::program_options;

class BurstBenchThreads
{
	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_bEnded = false;
	bool m_bPaused = false;

	std::vector<std::thread*> m_vThreads;
	std::atomic<unsigned long long> m_nWorkComplete = 0;
public:
	void SpawnThreads(const int nCount)
	{
		for (int n = 0; n < nCount; n++)
		{
			m_vThreads.push_back(new std::thread((std::bind(&BurstBenchThreads::work, this))));
		}
	}
	void JoinAll()
	{
		for (auto& i : m_vThreads)
		{
			i->join();
		}
	}
	void End()
	{
		if (m_bPaused)
		{
			Resume();
		}
		std::lock_guard<decltype(m_mutex)> lock(m_mutex);
		m_bEnded = true;
	}
	void Pause()
	{
		std::lock_guard<decltype(m_mutex)> lock(m_mutex);
		m_bPaused = true;
	}
	void Resume()
	{
		std::lock_guard<decltype(m_mutex)> lock(m_mutex);
		m_bPaused = false;
		m_cv.notify_all();
	}
	unsigned long long GetWorkComplete()
	{
		return m_nWorkComplete.load();
	}
private:
	void work()
	{
		while (!m_bEnded)
		{
			static md5wrapper md5;
			md5.getHashFromString(L"Hello world!");
			m_nWorkComplete++;
			if (m_bPaused)
			{
				std::unique_lock<decltype(m_mutex)> lock(m_mutex);
				m_cv.wait(lock, [this] { return !m_bPaused; });
			}
		}
	}
};

int main(int argc, char** argv)
{
	wprintf(L"BurstBench prototype v0.0.0.2 - (c)2022 Jeremy Collake - https://bitsum.com\n\n");

	static const int DEFAULT_THREAD_COUNT = -1;
	static const int DEFAULT_EXECUTION_TIME_BEFORE_SLEEP_MS = 10;
	static const int DEFAULT_SLEEP_MS = 10;
	static const int DEFAULT_ITERATIONS = 1000;

	int nThreads = DEFAULT_THREAD_COUNT;
	int nExecMS = DEFAULT_EXECUTION_TIME_BEFORE_SLEEP_MS;
	int nSleepMS = DEFAULT_SLEEP_MS;
	int nIterations = DEFAULT_ITERATIONS;

	// Arguments will be stored here
	std::string input;
	std::string output;
	// Configure options here
	po::options_description desc("Options");
	desc.add_options()
		("help,h", "show help")
		("threadcount,n", po::value(&nThreads), "Thread count (mandatory)")
		("execms,x", po::value(&nExecMS), "Execution time per interval (ms)")
		("sleepms,s", po::value(&nSleepMS), "Sleep time per interval (ms)")
		("iterations,i", po::value(&nIterations), "Iteration count");

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
	po::notify(vm);
	if (vm.count("help") || !vm.count("threadcount") || nThreads < 0)
	{
		std::cerr << desc << "\n";
		return 1;
	}

	wprintf(L" Options:\n"
		" %d threads\n"
		" %dms exec interval\n"
		" %dms sleep interval\n"
		" %d iterations (this will take %d seconds)\n\n", nThreads, nExecMS, nSleepMS, nIterations,
		((nSleepMS + nExecMS) * nIterations) / 1000);

	BurstBenchThreads burstBench;
	burstBench.SpawnThreads(nThreads);
	wprintf(L"Running. Press any key to abort on next iteration...\n");
	int nIterationsCompleted;
	for (nIterationsCompleted = 0; nIterationsCompleted < nIterations; nIterationsCompleted++)
	{
		burstBench.Resume();
		std::this_thread::sleep_for(std::chrono::milliseconds(nExecMS));
		burstBench.Pause();
		std::this_thread::sleep_for(std::chrono::milliseconds(nSleepMS));
		if (_kbhit())
		{
			wprintf(L"\n ABORTED!");
			break;
		}
	}
	burstBench.End();
	burstBench.JoinAll();

	auto nTotalWorkComplete = burstBench.GetWorkComplete();
	wprintf(L"\n"
		" Iterations Complete : %d\n"
		" Total Work Complete : %llu\n"
		" Average Per-Thread  : %llu\n",
		nIterationsCompleted,
		nTotalWorkComplete,
		nTotalWorkComplete / nThreads);

	wprintf(L"\nDone.");
	return 0;
}
