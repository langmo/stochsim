#pragma once
#include "stochsim_common.h"
#include <memory>
#include <fstream>
#include <vector>
#include <functional>
#include <filesystem>
#include "DelayReaction.h"
namespace stochsim
{
	class StatePropertyLogger :
		public ILogger
	{
	public:
		typedef std::function<size_t(const Molecule&)> LoggerFunction;
		StatePropertyLogger(std::string fileName, LoggerFunction loggerFunction = [](const Molecule& molecule)->size_t {return static_cast<size_t>(molecule[0]+0.5); }, size_t initialMaxValue = 10) : valueCounter_(initialMaxValue +1), fileName_(fileName), loggerFunction_(loggerFunction)
		{
		}

		virtual ~StatePropertyLogger()
		{
			if (file_)
			{
				file_->close();
				file_.reset();
			}
		}
		virtual bool WritesToDisk() const override
		{
			return true;
		}
		void LogProperty(const Molecule& molecule, double time)
		{
			auto id = loggerFunction_(molecule);
			while (id >= valueCounter_.size())
			{
				valueCounter_.resize(2 * valueCounter_.size());
			}
			valueCounter_[id]++;
		}
		virtual void WriteLog(ISimInfo& simInfo, double time) override
		{
			*file_ << time;
			for (auto& numMolecules : valueCounter_)
			{
				*file_ << ", " << numMolecules;
				numMolecules = 0;
			}
			*file_ << std::endl;
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			if (file_)
			{
				file_->close();
				file_.reset();
			}
			std::filesystem::path filePath{simInfo.GetSaveFolder()};
			filePath /= fileName_;

			file_ = std::make_unique<std::ofstream>();
			file_->open(filePath.string().c_str());
			if (!file_->is_open())
			{
				std::string errorMessage = "Could not open file ";
				errorMessage += filePath.string();
				throw std::runtime_error(errorMessage.c_str());
			}

			*file_ << "Time";
			for (std::vector<unsigned long>::size_type i = 0; i < valueCounter_.size(); i++)
			{
				valueCounter_[i] = 0;
				*file_ << ", value" << i;
			}
			*file_ << std::endl;
		}
		virtual void Uninitialize(ISimInfo& simInfo) override
		{
			if (file_)
			{
				file_->close();
				file_.reset();
			}
		}

	private:
		std::unique_ptr<std::ofstream> file_;
		std::string fileName_;
		std::vector<unsigned long> valueCounter_;
		LoggerFunction loggerFunction_;
	};
}
