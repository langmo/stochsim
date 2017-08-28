#include "Simulation.h"
#include <math.h>    
#include <cassert>
#include <sstream> 
#include <ctime>
#if defined(_WIN32)
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files:
#include <SDKDDKVer.h>
#include <windows.h>
#endif
namespace stochsim
{
	std::string CreatePathRecursively(std::string rawPath);

	class SimulationLogger : public Logger
	{
	public:
		SimulationLogger() : logPeriod_(0.1), baseFolder_("simulations/")
		{
		}
		virtual void AddTask(std::shared_ptr<LoggerTask> task) override
		{
			tasks_.push_back(std::move(task));
		}
		void Initialize(double time)
		{
			time_t t = std::time(0);   // get time now
			struct tm * now = localtime(&t);
			std::stringstream buffer;
			buffer << baseFolder_ << "/"
				<< (now->tm_year + 1900) << '-'
				<< (now->tm_mon + 1) << '-'
				<< now->tm_mday << '_'
				<< now->tm_hour << '-'
				<< now->tm_min << '-'
				<< now->tm_sec << '/';
			std::string folder = buffer.str();
			folder = CreatePathRecursively(folder);

			for (auto& task : tasks_)
			{
				task->Initialize(folder);
			}
			WriteLog(time);
			lastLogTime_ = time;
		}
		void Uninitialize(double time)
		{
			NotifyNextChange(time);
			WriteLog(time);
			lastLogTime_ = time;
			for (auto& task : tasks_)
			{
				task->Uninitialize();
			}
		}
		void NotifyNextChange(double time)
		{
			while (lastLogTime_ + logPeriod_ < time)
			{
				lastLogTime_ += logPeriod_;
				WriteLog(lastLogTime_);
			}
		}
		virtual void SetLogPeriod(double logPeriod) override
		{
			assert(logPeriod > 0);
			logPeriod_ = logPeriod;
		}
		virtual void SetBaseFolder(std::string baseFolder) override
		{
			baseFolder_ = std::move(baseFolder);
		}
	private:
		inline void WriteLog(double time)
		{
			for (auto& task : tasks_)
			{
				task->WriteLog(time);
			}
		}
		std::vector<std::shared_ptr<LoggerTask>> tasks_;
		double lastLogTime_;
		double logPeriod_;
		std::string baseFolder_;
	};
	
	class Simulation::Impl : public SimInfo
	{
	public:
		Impl() : randomEngine_(std::random_device{}()) {}
		~Impl() {}
		void Run(double maxTime)
		{
			/**
			** Run a modified version of Gillespies algorithm. The base algorithm is implemented as outlined in
			** Gillespie, Daniel T. "Exact stochastic simulation of coupled chemical reactions." The journal of physical chemistry 81.25 (1977): 2340-2361.
			** What we added is the support of fixed time delays, modelled roughly as reactions which fire at a specific time, instead of having a continuous propensity.
			**/

			time_ = 0;

			// Initialize
			for (auto& state : states_)
			{
				state->Initialize(*this);
			}
			logger_.Initialize(time_);

			// propensities of reactions
			std::vector<double> ai(propensityReactions_.size());

			// iterate
			while (time_ <= maxTime)
			{
				// Calculate aggregated reaction probability
				double a0 = 0;
				for (size_t i = 0; i < propensityReactions_.size(); i++)
				{
					ai[i] = propensityReactions_[i]->ComputeRate();
					a0 += ai[i];
				}

				// Calculate time span to next propensity reaction event
				double tau;
				if (a0 > 0)
				{
					double r1 = randomUniform_(randomEngine_);
					tau = 1 / a0 * log(1.0 / r1);
				}
				else
				{
					tau = stochsim::inf;
				}

				// Calculate time to next delayed reaction
				size_t nextDelayedIndex = 0;
				double nextDelayedT = stochsim::inf;
				for (size_t i = 0; i < delayedReactions_.size(); i++)
				{
					double temp = delayedReactions_[i]->NextReactionTime();
					if (temp < nextDelayedT)
					{
						nextDelayedT = temp;
						nextDelayedIndex = i;
					}
				}

				// Fire either next delayed or next propensity reaction, whichever is earlier
				if (nextDelayedT > time_ + tau)
				{
					// Fire a propensity reaction
					time_ += tau;
					if (time_ > maxTime)
					{
						time_ = maxTime;
						break;
					}

					// notify logger about the time of the next reaction event
					logger_.NotifyNextChange(time_);

					// decide on identity of next reaction event and fire this event
					double r2 = randomUniform_(randomEngine_);
					double afraction = r2 * a0;
					double asum = 0;
					for (size_t i = 0; i < propensityReactions_.size(); i++)
					{
						asum += ai[i];
						if (asum >= afraction)
						{
							propensityReactions_[i]->Fire(*this);
							break;
						}
					}
				}
				else
				{
					time_ = nextDelayedT;
					if (time_ > maxTime)
					{
						time_ = maxTime;
						break;
					}
					// notify logger about the time of the next reaction event
					logger_.NotifyNextChange(time_);
					delayedReactions_[nextDelayedIndex]->Fire(*this);
				}
			}

			// Uninitialize
			logger_.Uninitialize(time_);
			for (auto& state : states_)
			{
				state->Uninitialize(*this);
			}
		}
		
		virtual double SimTime() const override
		{
			return time_;
		}
		
		virtual unsigned long Rand(unsigned long lower, unsigned long upper) override
		{
			std::uniform_int_distribution<unsigned long> randomIndex(lower, upper);
			return randomIndex(randomEngine_);
		}

		virtual double Rand() override
		{
			return randomUniform_(randomEngine_);
		}

		Logger& GetLogger()
		{
			return logger_;
		}

		void AddReaction(std::shared_ptr<PropensityReaction> reaction)
		{
			propensityReactions_.push_back(std::move(reaction));
		}
		void AddReaction(std::shared_ptr<DelayedReaction> reaction)
		{
			delayedReactions_.push_back(std::move(reaction));
		}
		void AddState(std::shared_ptr<State> state)
		{
			states_.push_back(std::move(state));
		}

	private:
		std::vector<std::shared_ptr<PropensityReaction>> propensityReactions_;
		std::vector<std::shared_ptr<DelayedReaction>> delayedReactions_;
		std::vector<std::shared_ptr<State>> states_;
		double time_;
		SimulationLogger logger_;
		std::default_random_engine randomEngine_;
		// function to generate uniformly distributed random numbers in [0,1)
		std::uniform_real<double> randomUniform_;
	};

	Simulation::Simulation() : impl_(new Simulation::Impl())
	{
	}


	Simulation::~Simulation()
	{
		delete impl_;
		impl_ = nullptr;
	}

	void Simulation::AddReaction(std::shared_ptr<PropensityReaction> reaction)
	{
		impl_->AddReaction(std::move(reaction));
	}
	void Simulation::AddReaction(std::shared_ptr<DelayedReaction> reaction)
	{
		impl_->AddReaction(std::move(reaction));
	}
	void Simulation::AddState(std::shared_ptr<State> state)
	{
		impl_->AddState(std::move(state));
	}

	void Simulation::Run(double maxTime)
	{
		impl_->Run(maxTime);
	}

	Logger & Simulation::GetLogger()
	{
		return impl_->GetLogger();
	}

	std::string CreatePathRecursively(std::string rawPath)
	{
#if defined(_WIN32)
		std::wstring wRawPath = std::wstring(rawPath.begin(), rawPath.end());
		const wchar_t *path = wRawPath.c_str();

		wchar_t folder[MAX_PATH];
		ZeroMemory(folder, MAX_PATH * sizeof(wchar_t));

		const wchar_t *end;
		const wchar_t *end1;
		const wchar_t *end2;
		end1 = wcschr(path, L'\\');
		end2 = wcschr(path, L'/');
		if (end1 == NULL)
			end = end2;
		else if (end2 == NULL)
			end = end1;
		else
			end = end1 < end2 ? end1 : end2;

		while (end != NULL)
		{
			wcsncpy_s(folder, MAX_PATH, path, end - path + 1);
			if (!CreateDirectory(folder, NULL))
			{
				DWORD err = GetLastError();

				if (err != ERROR_ALREADY_EXISTS)
				{
					std::string errorMessage = "Could not create folder ";
					errorMessage += rawPath;
					errorMessage += " to store results.";
					throw std::exception(errorMessage.c_str());
				}
			}
			end1 = wcschr(end + 1, L'\\');
			end2 = wcschr(end + 1, L'/');
			if (end1 == NULL)
				end = end2;
			else if (end2 == NULL)
				end = end1;
		}
		return rawPath;
#else
		// OK, this should work somewhat differently on other systems. Since I don't know how to recursively create folders on other systems, simply save everything in current path...
		return "";
#endif
	}
}
