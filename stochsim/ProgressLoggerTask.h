#pragma once
#include "types.h"
#include <iostream>
#include <iomanip>
namespace stochsim
{
	/// <summary>
	/// Simple logger task which displays the fraction of the simulation which is already finished in the console.
	/// </summary>
	class ProgressLoggerTask :
		public LoggerTask
	{
	public:
		ProgressLoggerTask() : runtime_(1)
		{
		}
		virtual void WriteLog(double time) override
		{
			int progress = (int)(time / runtime_ * 100);
			std::cout << "\b\b\b\b\b\b" << std::setw(5) << std::fixed << std::setprecision(1) << (time / runtime_ * 100) << '%';
		}
		virtual void Initialize(std::string baseFolder, SimInfo& simInfo) override
		{
			runtime_ = simInfo.RunTime();
			std::cout << "Simulating model:   0.0%";
		}
		virtual void Uninitialize() override
		{
			std::cout << "\b\b\b\b\b\b Finished!" << std::endl;
		}

	private:
		double runtime_;
	};
}

