#pragma once
#include <memory>
#include <vector>
#include "stochsim_interfaces.h"
#include <fstream>
namespace stochsim
{
	/// <summary>
	/// A logger task which writes the concentration of all its supplied states to the disk in form of a table.
	/// </summary>
	class StateLogger :
		public ILogger
	{
	public:
		StateLogger(std::string fileName) : fileName_(fileName)
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
		
		virtual void WriteLog(double time) override
		{
			(*file_) << time;
			for (const auto& state : states_)
			{
				(*file_) << "," << state->Num();
			}
			(*file_) << std::endl;
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
		virtual void Initialize(std::string baseFolder, ISimInfo& simInfo) override
		{
			if (file_)
			{
				file_->close();
				file_.reset();
			}
			std::string fileName = baseFolder;
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

			(*file_) << "Time";
			for (const auto& state : states_)
			{
				(*file_) << ',' << state->Name();
			}
			(*file_) << std::endl;
		}
		virtual void Uninitialize() override
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
	};
}