#pragma once
#include "types.h"
#include "ComplexState.h"
#include <memory>
#include <fstream>
namespace stochsim
{
	/// <summary>
	/// Logger which writes some summary of the properties of the molecules represented by a complex state to a file.
	/// </summary>
	template<class T> class ComplexStateLoggerTask :
		public LoggerTask
	{
	public:
		typedef std::function<void(std::ostream& out, ComplexState<T>& state, double time)> LogFunc;

		ComplexStateLoggerTask(std::string fileName, std::shared_ptr<ComplexState<T>> state, LogFunc logger);
		virtual ~ComplexStateLoggerTask();

		virtual void WriteLog(double time) override;
		virtual void Initialize(std::string baseFolder, SimInfo& simInfo) override;
		virtual void Uninitialize() override;
	private:
		std::shared_ptr<ComplexState<T>> state_;
		LogFunc logger_;
		std::unique_ptr<std::ofstream> file_;
		std::string fileName_;
	};

	template<class T>
	inline ComplexStateLoggerTask<T>::ComplexStateLoggerTask(std::string fileName, std::shared_ptr<ComplexState<T>> state, LogFunc logger) : state_(std::move(state)), logger_(logger), fileName_(fileName)
	{
	}

	template<class T>
	inline ComplexStateLoggerTask<T>::~ComplexStateLoggerTask()
	{
		if (file_)
		{
			file_->close();
			file_.reset();
		}
	}

	template<class T>
	inline void ComplexStateLoggerTask<T>::WriteLog(double time)
	{
		(*file_) << time << ',';
		logger_(*file_, *state_, time);
		(*file_) << std::endl;
	}

	template<class T>
	inline void ComplexStateLoggerTask<T>::Initialize(std::string baseFolder, SimInfo& simInfo)
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
	}

	template<class T>
	inline void ComplexStateLoggerTask<T>::Uninitialize()
	{
		if (file_)
		{
			file_->close();
			file_.reset();
		}
	}
}