#pragma once
#include "mex.h"
#include <string>
#include <sstream>
#include <functional>
class MatlabParams
{
public:

	typedef std::unique_ptr<mxArray, std::function<void(mxArray*)>> MatlabVariable;

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
	unsigned int NumReturns()
	{
		return nlhs_;
	}
	static MatlabVariable CreateDoubleMatrix(mwSize rows, mwSize columns)
	{
		return MatlabVariable(::mxCreateDoubleMatrix(rows, columns, ::mxREAL), [](mxArray* content) {::mxDestroyArray(content); });
	}
	static MatlabVariable CreateCell(mwSize rows, mwSize columns)
	{
		mwSize dims[2];
		dims[0] = rows;
		dims[1] = columns;
		return MatlabVariable(::mxCreateCellArray(2, dims), [](mxArray* content) {::mxDestroyArray(content); });
	}
	static void AssignArrayElement(mxArray& array, mwIndex row, mwIndex column, double value)
	{
		if (!::mxIsDouble(&array))
		{
			std::stringstream errorMessage;
			errorMessage << "Matlab array is not a double array, it's a "<< mxGetClassName(&array) << " array.";
			throw std::exception(errorMessage.str().c_str());
		}
		if (::mxGetNumberOfDimensions(&array) != 2)
			throw std::exception("Only 2D cell arrays can be assigned to.");
		const mwSize* size = ::mxGetDimensions(&array);
		if (row < 0 || row >= size[0])
			throw std::exception("Invalid row index.");
		if (column < 0 || column >= size[1])
			throw std::exception("Invalid column index.");
		mwIndex dim[] = { row,  column };
		mwIndex index = ::mxCalcSingleSubscript(&array, 2, dim);
		double* content = mxGetPr(&array);
		if (content == NULL)
			throw std::exception("Matlab array is not a double array");
		content[index] = value;
	}
	static void AssignCellElement(mxArray& cell, mwIndex row, mwIndex column, std::string value)
	{
		if (!::mxIsCell(&cell))
		{
			std::stringstream errorMessage;
			errorMessage << "Matlab array is not a cell array, it's a " << mxGetClassName(&cell) << " array.";
			throw std::exception(errorMessage.str().c_str());
		}
		if (::mxGetNumberOfDimensions(&cell) != 2)
			throw std::exception("Only 2D cell arrays can be assigned to.");
		const mwSize* size = ::mxGetDimensions(&cell);
		if (row < 0 || row >= size[0])
			throw std::exception("Invalid row index.");
		if (column < 0 || column >= size[1])
			throw std::exception("Invalid column index.");
		mwIndex dim[] =  {row,  column};
		mwIndex index = ::mxCalcSingleSubscript(&cell, 2, dim);
		::mxSetCell(&cell, index, ::mxCreateString(value.c_str()));
	}

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

	bool IsString(int index)
	{
		index += shift_;
		AssertParamIndex(index);
		return ::mxIsChar(prhs_[index]);
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
	template<> void Set<MatlabVariable>(int index, MatlabVariable&& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = value.release();
	}
	template<> void Set<MatlabVariable&>(int index, MatlabVariable& value)
	{
		if (index >= nlhs_)
			return;
		plhs_[index] = value.release();
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

