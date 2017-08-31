#pragma once
#include "stochsim_interfaces.h"
#include <functional>
#include <memory>
#include <fstream>
namespace stochsim
{
	class CustomLogger :
		public ILogger
	{
	public:
		typedef std::function<void(std::ostream& out)> HeaderFunc;
		typedef std::function<void(std::ostream& out, double time)> LogFunc;

		CustomLogger(std::string fileName, HeaderFunc headerFunc, LogFunc logFunc) : headerFunc_(headerFunc), logFunc_(logFunc), fileName_(fileName)
		{
		}
		virtual ~CustomLogger()
		{
			if (file_)
			{
				file_->close();
				file_.reset();
			}
		}

		virtual void WriteLog(double time) override
		{
			logFunc_(*file_, time);
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

			headerFunc_(*file_);
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
		HeaderFunc headerFunc_;
		LogFunc logFunc_;
		std::unique_ptr<std::ofstream> file_;
		std::string fileName_;
	};
}
