#include "Simulation.h"
#include <math.h>    
#include <cassert>
#include <sstream> 
#include <ctime>
#include <locale>
#include <codecvt>
#include <iostream>
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

	class LogManager
	{
	public:
		LogManager() : logPeriod_(1.0), baseFolder_("simulations"), uniqueSubFolder_(true)
		{
		}
		void SetUniqueSubfolder(bool uniqueSubFolder)
		{
			uniqueSubFolder_ = uniqueSubFolder;
		}
		bool IsUniqueSubfolder() const
		{
			return uniqueSubFolder_;
		}
		double GetLogPeriod() const
		{
			return logPeriod_;
		}
		std::string GetBaseFolder() const 
		{
			return baseFolder_;
		}

		void AddTask(std::shared_ptr<ILogger> task)
		{
			tasks_.push_back(std::move(task));
		}
		void Initialize(double time, ISimInfo& simInfo)
		{
			// Test if any logger is writing anything to the disk, i.e. if we have to create a results folder at all...
			bool shouldCreate = false;
			for (auto& task : tasks_)
			{
				if (task->WritesToDisk())
				{
					shouldCreate = true;
					break;
				}
			}
			std::string folder;
			if (shouldCreate)
			{
				time_t t = std::time(0);
				struct tm * now = localtime(&t);
				std::stringstream buffer;
				buffer << baseFolder_;
				if (uniqueSubFolder_)
				{
					buffer << "/"
						<< (now->tm_year + 1900) << '-'
						<< (now->tm_mon + 1) << '-'
						<< now->tm_mday << '_'
						<< now->tm_hour << '-'
						<< now->tm_min << '-'
						<< now->tm_sec << '/';
				}
				folder = buffer.str();
				folder = CreatePathRecursively(folder);
			}
			else
				folder = "";

			for (auto& task : tasks_)
			{
				task->Initialize(folder, simInfo);
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
		void SetLogPeriod(double logPeriod)
		{
			assert(logPeriod > 0);
			logPeriod_ = logPeriod;
		}
		void SetBaseFolder(std::string baseFolder)
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
		std::vector<std::shared_ptr<ILogger>> tasks_;
		double lastLogTime_;
		double logPeriod_;
		std::string baseFolder_;
		bool uniqueSubFolder_;
	};
	
	class Simulation::Impl : public ISimInfo
	{
	public:
		Impl() : randomEngine_(std::random_device{}()), time_(0), runtime_(0)
		{
		}
		~Impl() {}
		void Run(double runtime)
		{
			/**
			** Run a modified version of Gillespies algorithm. The base algorithm is implemented as outlined in
			** Gillespie, Daniel T. "Exact stochastic simulation of coupled chemical reactions." The journal of physical chemistry 81.25 (1977): 2340-2361.
			** What we added is the support of fixed time delays, modelled roughly as reactions which fire at a specific time, instead of having a continuous propensity.
			**/
			runtime_ = runtime;
			time_ = 0;

			// Initialize
			for (auto& state : states_)
			{
				state->Initialize(*this);
			}
			for (auto& reaction : propensityReactions_)
			{
				reaction->Initialize(*this);
			}
			for (auto& reaction : delayedReactions_)
			{
				reaction->Initialize(*this);
			}
			logger_.Initialize(time_, *this);

			// propensities of reactions
			std::vector<double> ai(propensityReactions_.size());

			// iterate
			while (time_ <= runtime)
			{
				// Calculate aggregated reaction probability
				double a0 = 0;
				for (size_t i = 0; i < propensityReactions_.size(); i++)
				{
					ai[i] = propensityReactions_[i]->ComputeRate(*this);
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
					double temp = delayedReactions_[i]->NextReactionTime(*this);
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
					if (time_ > runtime)
					{
						time_ = runtime;
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
					if (time_ > runtime)
					{
						time_ = runtime;
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
		virtual double LogPeriod() const override
		{
			return logger_.GetLogPeriod();
		}
		virtual double RunTime() const override
		{
			return runtime_;
		}
		virtual size_t Rand(size_t lower, size_t upper) override
		{
			std::uniform_int_distribution<size_t> randomIndex(lower, upper);
			return randomIndex(randomEngine_);
		}

		virtual double Rand() override
		{
			return randomUniform_(randomEngine_);
		}

		LogManager& GetLogger()
		{
			return logger_;
		}

		void AddReaction(std::shared_ptr<IPropensityReaction> reaction)
		{
			if (GetPropensityReaction(reaction->GetName()) || GetDelayedReaction(reaction->GetName()))
			{
				std::stringstream errorMessage;
				errorMessage << "Reaction with name " << reaction->GetName() << " already exists in simulation.";
				throw std::exception(errorMessage.str().c_str());
			}
			propensityReactions_.push_back(std::move(reaction));
		}
		void AddReaction(std::shared_ptr<IDelayedReaction> reaction)
		{
			if (GetPropensityReaction(reaction->GetName()) || GetDelayedReaction(reaction->GetName()))
			{
				std::stringstream errorMessage;
				errorMessage << "Reaction with name "<<reaction->GetName()<< " already exists in simulation.";
				throw std::exception(errorMessage.str().c_str());
			}
			delayedReactions_.push_back(std::move(reaction));
		}
		void AddState(std::shared_ptr<IState> state)
		{
			if (GetState(state->GetName()))
			{
				std::stringstream errorMessage;
				errorMessage << "State with name " << state->GetName() << " already exists in simulation.";
				throw std::exception(errorMessage.str().c_str());
			}
			states_.push_back(std::move(state));
		}

		std::shared_ptr<IState> GetState(const std::string & name)
		{
			for (std::shared_ptr<IState>& state : states_)
			{
				if (state->GetName() == name)
					return state;
			}
			return nullptr;
		}
		Simulation::Collection<std::shared_ptr<IState>>&  GetStates()
		{
			return states_;
		}
		std::shared_ptr<IPropensityReaction> GetPropensityReaction(const std::string & name)
		{
			for (std::shared_ptr<IPropensityReaction>& propensityReaction : propensityReactions_)
			{
				if (propensityReaction->GetName() == name)
					return propensityReaction;
			}
			return nullptr;
		}
		std::shared_ptr<IDelayedReaction> GetDelayedReaction(const std::string& name)
		{
			for (std::shared_ptr<IDelayedReaction>& delayedReaction : delayedReactions_)
			{
				if (delayedReaction->GetName() == name)
					return delayedReaction;
			}
			return nullptr;
		}

	private:
		std::vector<std::shared_ptr<IPropensityReaction>> propensityReactions_;
		std::vector<std::shared_ptr<IDelayedReaction>> delayedReactions_;
		std::vector<std::shared_ptr<IState>> states_;
		double time_;
		double runtime_;
		LogManager logger_;
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

	void Simulation::AddReaction(std::shared_ptr<IPropensityReaction> reaction)
	{
		impl_->AddReaction(std::move(reaction));
	}
	void Simulation::AddReaction(std::shared_ptr<IDelayedReaction> reaction)
	{
		impl_->AddReaction(std::move(reaction));
	}
	void Simulation::AddState(std::shared_ptr<IState> state)
	{
		impl_->AddState(std::move(state));
	}

	std::shared_ptr<IState> Simulation::GetState(const std::string & name)
	{
		return impl_->GetState(name);
	}

	const Simulation::Collection<std::shared_ptr<IState>>& Simulation::GetStates()
	{
		return impl_->GetStates();
	}

	std::shared_ptr<IPropensityReaction> Simulation::GetPropensityReaction(const std::string & name)
	{
		return impl_->GetPropensityReaction(name);
	}

	std::shared_ptr<IDelayedReaction> Simulation::GetDelayedReaction(const std::string & name)
	{
		return impl_->GetDelayedReaction(name);
	}

	void Simulation::Run(double maxTime)
	{
		impl_->Run(maxTime);
	}

	void Simulation::AddLogger(std::shared_ptr<ILogger> logger)
	{
		impl_->GetLogger().AddTask(logger);
	}
	void Simulation::SetLogPeriod(double logPeriod)
	{
		impl_->GetLogger().SetLogPeriod(logPeriod);
	}
	double Simulation::GetLogPeriod() const
	{
		return impl_->GetLogger().GetLogPeriod();
	}
	void Simulation::SetBaseFolder(std::string baseFolder)
	{
		impl_->GetLogger().SetBaseFolder(baseFolder);
	}
	std::string Simulation::GetBaseFolder() const
	{
		return impl_->GetLogger().GetBaseFolder();
	}
	void Simulation::SetUniqueSubfolder(bool uniqueSubFolder)
	{
		impl_->GetLogger().SetUniqueSubfolder(uniqueSubFolder);
	}
	bool Simulation::IsUniqueSubfolder() const
	{
		return impl_->GetLogger().IsUniqueSubfolder();
	}




	std::wstring s2ws(const std::string& str)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	std::string ws2s(const std::wstring& wstr)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
	std::string CreatePathRecursively(std::string rawPath)
	{
#if defined(_WIN32)
		std::wstring wRawPath = s2ws(rawPath);
		// remove trailing / or \ 
		while (wRawPath[wRawPath.size() - 1] == L'\\' || wRawPath[wRawPath.size() - 1] == L'/')
			wRawPath.resize(wRawPath.size()-1);
		std::wstring::size_type pos = 0;
		while (true)
		{
			std::wstring::size_type end1 = wRawPath.find(L'\\', pos);
			std::wstring::size_type end2 = wRawPath.find(L'/', pos);
			std::wstring::size_type end = end1 < end2 ? end1 : end2;
			std::wstring folder = wRawPath.substr(0, end);
			if (folder[folder.size() - 1] != ':')
			{
				if (!CreateDirectory(folder.c_str(), NULL))
				{
					DWORD err = GetLastError();

					if (err != ERROR_ALREADY_EXISTS)
					{
						std::stringstream errorMessage;
						errorMessage << "Could not create folder ";
						errorMessage << rawPath;
						errorMessage << " to store results. Iteration failed at sub-folder ";
						errorMessage << ws2s(folder);
						throw std::exception(errorMessage.str().c_str());
					}
				}
			}
			if (end == std::wstring::npos)
				break;
			pos = end + 1;
		}
		return rawPath;
#else
		// TODO: Since I don't know how to recursively create folders on other systems, I simply save everything in current path, but this should be fixed somehow...
		return "";
#endif
	}
}
