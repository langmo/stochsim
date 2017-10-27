#pragma once
#include "stochsim_common.h"
#include <memory>
#include <fstream>
#include <vector>
#include "DelayReaction.h"
namespace stochsim
{
	class ComposedStateLogger :
		public ILogger
	{
	public:

		ComposedStateLogger(std::string fileName, size_t initialMaxModified) : modificationCounter_(initialMaxModified), fileName_(fileName)
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
		void RemoveListener(ComposedState::Molecule& molecule, double time)
		{
			while (molecule.numModified > modificationCounter_.size())
			{
				modificationCounter_.resize(2 * modificationCounter_.size());
			}
			modificationCounter_[molecule.numModified]++;
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
	};
}
