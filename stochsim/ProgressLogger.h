#pragma once
#include "stochsim.h"
#include <iostream>
#include <iomanip>
namespace stochsim
{
	/// <summary>
	/// Simple logger task which displays the fraction of the simulation which is already finished in the console.
	/// </summary>
	class ProgressLogger :
		public ILogger
	{
	public:
		ProgressLogger() : runtime_(1)
		{
		}
		virtual void WriteLog(double time) override
		{
			std::cout << "\b\b\b\b\b\b" << std::setw(5) << std::fixed << std::setprecision(1) << (time / runtime_ * 100) << '%';
		}
		virtual void Initialize(std::string baseFolder, ISimInfo& simInfo) override
		{
			runtime_ = simInfo.RunTime();
			std::cout << "Simulating model:   0.0%";
		}
		virtual void Uninitialize() override
		{
			std::cout << "\b\b\b\b\b\b Finished!" << std::endl;
		}
		virtual bool WritesToDisk() const override
		{
			return false;
		}
	private:
		double runtime_;
	};
}

