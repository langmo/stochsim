#pragma once
#include "mex.h"
#include <string>
#include <sstream>
#include <type_traits>
#include <functional>
#include <unordered_map>
#include <vector>
class MatlabParams
{
public:

	typedef std::unique_ptr<mxArray, std::function<void(mxArray*)>> MatlabVariable;
	typedef std::unordered_map<std::string, double> MatlabStruct;
	MatlabParams(size_t nlhs, mxArray* plhs[], size_t nrhs, const mxArray *prhs[], size_t shift=0) : shift_(shift), nlhs_(nlhs), plhs_(plhs), nrhs_(nrhs), prhs_(prhs)
	{
	}

	~MatlabParams()
	{
	}

	size_t NumParams()
	{
		return nrhs_ - shift_;
	}
	size_t NumReturns()
	{
		return nlhs_;
	}
	static MatlabVariable CreateDoubleMatrix(mwSize rows, mwSize columns)
	{
		return MatlabVariable(::mxCreateDoubleMatrix(rows, columns, ::mxREAL), 
			[](mxArray* content) 
			{
				//TODO: I think this should be destroyed, however, it induces Matlab to crash...
				//if(content)
				//	::mxDestroyArray(content); 
				content = nullptr;
			});
	}
	static MatlabVariable CreateCell(mwSize rows, mwSize columns)
	{
		mwSize dims[2];
		dims[0] = rows;
		dims[1] = columns;
		return MatlabVariable(::mxCreateCellArray(2, dims),
			[](mxArray* content)
		{
			//TODO: I think this should be destroyed, however, it induces Matlab to crash...
			//if (content)
			//	::mxDestroyArray(content);
			content = nullptr;
		});
	}
	static void AssignArrayElement(mxArray& array, mwIndex row, mwIndex column, double value)
	{
		if (!::mxIsDouble(&array))
		{
			std::stringstream errorMessage;
			errorMessage << "Matlab array is not a double array, it's a "<< mxGetClassName(&array) << " array.";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		if (::mxGetNumberOfDimensions(&array) != 2)
			throw std::runtime_error("Only 2D cell arrays can be assigned to.");
		const mwSize* size = ::mxGetDimensions(&array);
		if (row < 0 || row >= size[0])
			throw std::runtime_error("Invalid row index.");
		if (column < 0 || column >= size[1])
			throw std::runtime_error("Invalid column index.");
		mwIndex dim[] = { row,  column };
		mwIndex index = ::mxCalcSingleSubscript(&array, 2, dim);
		double* content = mxGetPr(&array);
		if (content == NULL)
			throw std::runtime_error("Matlab array is not a double array");
		content[index] = value;
	}
	static void AssignCellElement(mxArray& cell, mwIndex row, mwIndex column, std::string value)
	{
		if (!::mxIsCell(&cell))
		{
			std::stringstream errorMessage;
			errorMessage << "Matlab array is not a cell array, it's a " << mxGetClassName(&cell) << " array.";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		if (::mxGetNumberOfDimensions(&cell) != 2)
			throw std::runtime_error("Only 2D cell arrays can be assigned to.");
		const mwSize* size = ::mxGetDimensions(&cell);
		if (row < 0 || row >= size[0])
			throw std::runtime_error("Invalid row index.");
		if (column < 0 || column >= size[1])
			throw std::runtime_error("Invalid column index.");
		mwIndex dim[] =  {row,  column};
		mwIndex index = ::mxCalcSingleSubscript(&cell, 2, dim);
		::mxSetCell(&cell, index, ::mxCreateString(value.c_str()));
	}
	static void AssignCellElement(mxArray& cell, mwIndex row, mwIndex column, MatlabVariable value)
	{
		if (!::mxIsCell(&cell))
		{
			std::stringstream errorMessage;
			errorMessage << "Matlab array is not a cell array, it's a " << mxGetClassName(&cell) << " array.";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		if (::mxGetNumberOfDimensions(&cell) != 2)
			throw std::runtime_error("Only 2D cell arrays can be assigned to.");
		const mwSize* size = ::mxGetDimensions(&cell);
		if (row < 0 || row >= size[0])
			throw std::runtime_error("Invalid row index.");
		if (column < 0 || column >= size[1])
			throw std::runtime_error("Invalid column index.");
		mwIndex dim[] = { row,  column };
		mwIndex index = ::mxCalcSingleSubscript(&cell, 2, dim);
		::mxSetCell(&cell, index, value.release());
	}
	template<typename T> inline T Get(size_t index)
	{
		static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "Function template T MatlabParams::Get<T>(size_t) not defined for type T.");
		if(std::is_floating_point<T>::value)
			return static_cast<T>(Get<double>(index));
		else
			return static_cast<T>(Get<double>(index)+0.5); // round to nearest integer value
	}
	

	bool IsString(size_t index)
	{
		index += shift_;
		AssertParamIndex(index);
		return ::mxIsChar(prhs_[index]);
	}

	MatlabParams ShiftInputs(size_t shift)
	{
		shift += shift_;
		AssertParamIndex(shift - 1);
		return MatlabParams(nlhs_, plhs_, nrhs_, prhs_, shift);
	}
	template<typename T> void Set(size_t index, T value)
	{
		static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "Function template void Set(size_t index, T value) not defined for type T.");
		if (index >= nlhs_)
			return;
		plhs_[index] = ::mxCreateDoubleScalar(static_cast<double>(value));
	}
	
private:
	char buffer_[256];
	const size_t shift_;
	const size_t nlhs_;
	mxArray** const plhs_;
	const size_t nrhs_;
	const mxArray ** const prhs_;
	void AssertParamIndex(size_t index)
	{
		if (index < 0)
		{
			throw std::runtime_error("Invalid negative index.");
		}
		else if (index >= nrhs_)
		{
			std::stringstream errorMessage;
			errorMessage << "At least " << (index + 1) << " parameters expected for function call, only " << nrhs_ << " were provided.";
			throw std::runtime_error(errorMessage.str().c_str());
		}
	}
};
/*
* Specializations of getters and setters.
* Must be defined outside of class due to some arcane template specialization rule,
* which was abolished in C++17 but gcc didn't care and still behaves non-conformant in C++17...
*/
template<> inline double MatlabParams::Get<double>(size_t index)
{
	index += shift_;
	AssertParamIndex(index);
	const mxArray* elem = prhs_[index];
	if (mxGetM(elem) != 1 || mxGetN(elem) != 1 || !(mxIsNumeric(elem) || mxIsLogical(elem)) || mxIsEmpty(elem) || mxIsComplex(elem))
	{
		std::stringstream errorMessage;
		errorMessage << "Parameter " << (index + 1) << " must be a noncomplex scalar number (e.g. a double).";
		throw std::runtime_error(errorMessage.str().c_str()); 
	}
	return mxGetScalar(elem);
}
template<> inline std::vector<double> MatlabParams::Get<std::vector<double>>(size_t index)
{
	std::vector<double> result;
	index += shift_;
	AssertParamIndex(index);
	const mxArray* elem = prhs_[index];
	if ((mxGetM(elem) != 1 && mxGetN(elem) != 1) || !(mxIsDouble(elem)) || mxIsEmpty(elem) || mxIsComplex(elem))
	{
		std::stringstream errorMessage;
		errorMessage << "Parameter " << (index + 1) << " must be a noncomplex scalar double row or column vector.";
		throw std::runtime_error(errorMessage.str().c_str());
	}
	auto elements = mxGetNumberOfElements(elem);
	auto pr = mxGetPr(elem);
	for (size_t j = 0; j < elements; j++) 
	{
		result.push_back(pr[j]);
	}
	return std::move(result);
}
template<> inline MatlabParams::MatlabStruct MatlabParams::Get<MatlabParams::MatlabStruct>(size_t index)
{
	index += shift_;
	AssertParamIndex(index);
	MatlabParams::MatlabStruct result;
	const mxArray* elem = prhs_[index];
	if (!mxIsStruct(elem) || mxGetM(elem) != 1 || mxGetN(elem) != 1)
	{
		std::stringstream errorMessage;
		errorMessage << "Parameter " << (index + 1) << " must be a struct.";
		throw std::runtime_error(errorMessage.str().c_str());
	}
	int numFields = ::mxGetNumberOfFields(elem);
	for (int i = 0; i < numFields; i++)
	{
		auto name = mxGetFieldNameByNumber(elem, i);
		mxArray* field = mxGetField(elem, 0, name);
		if (mxGetM(field) != 1 || mxGetN(field) != 1 || !(mxIsNumeric(field) || mxIsLogical(field)) || mxIsEmpty(field) || mxIsComplex(field))
		{
			std::stringstream errorMessage;
			errorMessage << "Field " << name << " of parameter " << (index + 1) << " must be a noncomplex scalar number (e.g. a double).";
			throw std::runtime_error(errorMessage.str().c_str());
		}
		auto value= mxGetScalar(field);
		result.emplace(name, value);
	}
	return std::move(result);
}
template<> inline const mxArray* MatlabParams::Get<const mxArray*>(size_t index)
{
	index += shift_;
	AssertParamIndex(index);
	return prhs_[index];
}
template<> inline bool MatlabParams::Get<bool>(size_t index)
{
	return Get<double>(index) != 0;
}
template<> inline std::string MatlabParams::Get<std::string>(size_t index)
{
	index += shift_;
	AssertParamIndex(index);

	if (mxGetString(prhs_[index], buffer_, sizeof(buffer_)))
	{
		std::stringstream errorMessage;
		errorMessage << "Parameter " << (index + 1) << " must be a string less than " << (sizeof(buffer_) - 1) << " characters long.";
		throw std::runtime_error(errorMessage.str().c_str());
	}
	return std::string(buffer_);
}
template<> inline void MatlabParams::Set<mxArray*>(size_t index, mxArray* value)
{
	if (index >= nlhs_)
		return;
	plhs_[index] = value;
}
template<> inline void MatlabParams::Set<std::string>(size_t index, std::string value)
{
	if (index >= nlhs_)
		return;
	plhs_[index] = ::mxCreateString(value.c_str());
}
template<> inline void MatlabParams::Set<const std::string&>(size_t index, const std::string& value)
{
	if (index >= nlhs_)
		return;
	plhs_[index] = ::mxCreateString(value.c_str());
}
template<> inline void MatlabParams::Set<bool>(size_t index, bool value)
{
	if (index >= nlhs_)
		return;
	plhs_[index] = ::mxCreateLogicalScalar(value);
}

