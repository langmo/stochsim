#pragma once
#include <memory>
#include <vector>
#include "stochsim_common.h"
#include <fstream>
#include <filesystem>
namespace stochsim
{
	/// <summary>
	/// A logger task which writes the concentration of all its supplied states to the disk in form of a table.
	/// </summary>
	class StateLogger :
		public ILogger
	{
	public:
		StateLogger(std::string fileName) : fileName_(fileName), shouldLog_(true)
		{
		}
		template <typename... T> StateLogger(std::string fileName, std::shared_ptr<IState> state, T... others) : StateLogger(fileName)
		{
			AddState(state, others...);
		}
		virtual ~StateLogger()
		{
			if (file_)
			{
				file_->close();
				file_.reset();
			}
		}
		virtual bool WritesToDisk() const override
		{
			return shouldLog_;
		}
		virtual void WriteLog(ISimInfo& simInfo, double time) override
		{
			if (!shouldLog_)
				return;
			(*file_) << time;
			for (const auto& state : states_)
			{
				(*file_) << "," << state->Num(simInfo);
			}
			(*file_) << std::endl;
		}

		void SetShouldLog(bool shouldLog)
		{
			shouldLog_ = shouldLog;
		}

		bool IsShouldLog() const
		{
			return shouldLog_;
		}

		void SetFileName(std::string filename)
		{
			fileName_ = std::move(filename);
		}

		std::string GetFileName() const
		{
			return fileName_;
		}

		void AddState(std::shared_ptr<IState> state)
		{
			states_.push_back(std::move(state));
		}
		template <typename... T> void AddState(std::shared_ptr<IState> state, T... others)
		{
			AddState(state);
			AddState(others...);
		}
		virtual void Initialize(ISimInfo& simInfo) override
		{
			if (!shouldLog_)
				return;
			if (file_)
			{
				file_->close();
				file_.reset();
			}
			std::error_code ec{};
			std::filesystem::path absolutePath{std::filesystem::weakly_canonical( simInfo.GetSaveFolder(), ec)};
			if(ec)
			{
				std::stringstream errorMessage;
				errorMessage << "Could not determine canonical path for save folder  \"" << simInfo.GetSaveFolder() << "\".";
				throw std::runtime_error(errorMessage.str().c_str());
			}
			absolutePath /= fileName_;

			file_ = std::make_unique<std::ofstream>();
			file_->open(absolutePath);
			if (!file_->is_open())
			{
				std::string errorMessage = "Could not open log file ";
				errorMessage += absolutePath.string();
				throw std::runtime_error(errorMessage.c_str());
			}

			(*file_) << "Time";
			for (const auto& state : states_)
			{
				(*file_) << ',' << state->GetName();
			}
			(*file_) << std::endl;
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
		std::vector<std::shared_ptr<IState>> states_;
		std::unique_ptr<std::ofstream> file_;
		std::string fileName_;
		bool shouldLog_;
	};
}