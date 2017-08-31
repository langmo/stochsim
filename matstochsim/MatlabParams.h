#pragma once
#include "mex.h"
#include <string>
#include <sstream>
class MatlabParams
{
public:

	MatlabParams(int nlhs, mxArray* plhs[], int nrhs, const mxArray *prhs[], unsigned int shift=0) : shift_(shift), nlhs_(nlhs), plhs_(plhs), nrhs_(nrhs), prhs_(prhs)
	{
	}

	~MatlabParams()
	{
	}

	unsigned int NumParams()
	{
		return nrhs_ - shift_;
	}

	/*std::string GetString(int index)
	{
		index += shift_;
		AssertParamIndex(index);

		if (mxGetString(prhs_[index], buffer_, sizeof(buffer_)))
		{
			std::stringstream errorMessage;
			errorMessage << "Parameter " << (index + 1) << " must be a string less than " << (sizeof(buffer_)-1) << " characters long.";
			throw std::exception(errorMessage.str().c_str());
		}
		return std::string(buffer_);
	}
	double GetDouble(int index)
	{
		index += shift_;
		AssertParamIndex(index);
		const mxArray* elem = prhs_[index];
		size_t mrows = mxGetM(elem);
		size_t ncols = mxGetN(elem);
		if (mxGetM(elem) != 1 || mxGetN(elem) != 1 || !(mxIsNumeric(elem)|| mxIsLogical(elem)) || mxIsEmpty(elem) || mxIsComplex(elem))
		{
			std::stringstream errorMessage;
			errorMessage << "Parameter " << (index + 1) << " must be a noncomplex scalar number.";
			throw std::exception(errorMessage.str().c_str());
		}
		return mxGetScalar(elem);
	}
	bool GetBool(int index)
	{
		return GetDouble(index) != 0;
	}
	template<typename T> T GetNumber(int index)
	{
		return static_cast<T>(GetDouble(index) + 0.5);
	}*/

	template<typename T> T Get(int index)
	{
		return static_cast<T>(Get<double>(index) + 0.5);
	}
	template<> double Get<double>(int index)
	{
		index += shift_;
		AssertParamIndex(index);
		const mxArray* elem = prhs_[index];
		size_t mrows = mxGetM(elem);
		size_t ncols = mxGetN(elem);
		if (mxGetM(elem) != 1 || mxGetN(elem) != 1 || !(mxIsNumeric(elem) || mxIsLogical(elem)) || mxIsEmpty(elem) || mxIsComplex(elem))
		{
			std::stringstream errorMessage;
			errorMessage << "Parameter " << (index + 1) << " must be a noncomplex scalar number.";
			throw std::exception(errorMessage.str().c_str());
		}
		return mxGetScalar(elem);
	}
	template<> bool Get<bool>(int index)
	{
		return Get<double>(index) != 0;
	}
	template<> std::string Get<std::string>(int index)
	{
		index += shift_;
		AssertParamIndex(index);

		if (mxGetString(prhs_[index], buffer_, sizeof(buffer_)))
		{
			std::stringstream errorMessage;
			errorMessage << "Parameter " << (index + 1) << " must be a string less than " << (sizeof(buffer_) - 1) << " characters long.";
			throw std::exception(errorMessage.str().c_str());
		}
		return std::string(buffer_);
	}


	MatlabParams ShiftInputs(int shift)
	{
		shift += shift_;
		AssertParamIndex(shift - 1);
		return MatlabParams(nlhs_, plhs_, nrhs_, prhs_, shift);
	}
	template<typename T> void Set(int index, T&& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = ::mxCreateDoubleScalar(static_cast<double>(value));
	}
	template<> void Set<std::string>(int index, std::string&& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = ::mxCreateString(value.c_str());
	}
	template<> void Set<std::string&>(int index, std::string& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = ::mxCreateString(value.c_str());
	}
	template<> void Set<bool>(int index, bool&& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = ::mxCreateLogicalScalar(value);
	}
	template<> void Set<bool&>(int index, bool& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = ::mxCreateLogicalScalar(value);
	}
private:
	char buffer_[256];
	const unsigned int shift_;
	const int nlhs_;
	mxArray** const plhs_;
	const int nrhs_;
	const mxArray ** const prhs_;
	void AssertParamIndex(int index)
	{
		if (index < 0)
		{
			throw std::exception("Invalid negative index.");
		}
		else if (index >= nrhs_)
		{
			std::stringstream errorMessage;
			errorMessage << "At least " << (index + 1) << " parameters expected for function call, only " << nrhs_ << " were provided.";
			throw std::exception(errorMessage.str().c_str());
		}
	}
};

