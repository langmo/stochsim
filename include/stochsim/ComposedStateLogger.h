#pragma once
#include "stochsim_common.h"
#include <memory>
#include <fstream>
#include <vector>
#include <functional>
#include "DelayReaction.h"
namespace stochsim
{
	class ComposedStateLogger :
		public ILogger
	{
	public:
		typedef std::function<size_t(const MoleculeProperties&)> LoggerFunction;
		ComposedStateLogger(std::string fileName, LoggerFunction loggerFunction = [](const MoleculeProperties& properties)->size_t {return static_cast<size_t>(properties[0]+0.5); }, size_t initialMaxModified = 10) : modificationCounter_(initialMaxModified), fileName_(fileName), loggerFunction_(loggerFunction)
		{
		}

		virtual ~ComposedStateLogger()
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
		void RemoveListener(const Molecule& molecule, double time)
		{
			auto id = loggerFunction_(molecule.properties);
			while (id > modificationCounter_.size())
			{
				modificationCounter_.resize(2 * modificationCounter_.size());
			}
			modificationCounter_[id]++;
		}
		virtual void WriteLog(ISimInfo& simInfo, double time) override
		{
			*file_ << time;
			for (auto& numMolecules : modificationCounter_)
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
			std::string fileName = simInfo.GetSaveFolder();
			fileName += "/";
			fileName += fileName_;

			file_ = std::make_unique<std::ofstream>();
			file_->open(fileName);
			if (!file_->is_open())
			{
				std::string errorMessage = "Could not open file ";
				errorMessage += fileName;
				throw new std::exception(errorMessage.c_str());
			}

			*file_ << "Time";
			for (std::vector<unsigned long>::size_type i = 0; i < modificationCounter_.size(); i++)
			{
				modificationCounter_[i] = 0;
				*file_ << ", M" << i;
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
		std::vector<unsigned long> modificationCounter_;
		LoggerFunction loggerFunction_;
	};
}
