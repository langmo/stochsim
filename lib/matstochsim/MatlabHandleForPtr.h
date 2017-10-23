#pragma once
#include "mex.h"
#include <stdint.h>
#include <string>
#include <cstring>
#include <typeinfo>

template<class Base> mxArray* CreateMatlabHandle(Base *ptr);

/// <summary>
/// Helper class holding a pointer, and which provides some bare minimum of type safety when
/// wanting to have a handle in Matlab for a C++ (pointer to an) object.
/// Do not construct directly, but use CreateMatlabHandle to encapsulate a pointer into a handle which can
/// be passed to Matlab, GetPtrFromMatlabHandle to retrieve the pointer from this handle, and DeleteMatlabHandle
/// to destroy the handle as well as the pointed to object.
/// </summary>
template<class Base> class MatlabHandleForPtr
{
private: 
	static constexpr uint32_t classHandleSignature = 0xFF00F0A5;
	// Only allow implicit construction via ConvertPtr2Mat.
    MatlabHandleForPtr(Base *ptr) : basePointer_(ptr), className_(typeid(Base).name()), signature_(classHandleSignature)
	{ 
	}
	// Prevent copying
	MatlabHandleForPtr(const MatlabHandleForPtr&) = delete;
	MatlabHandleForPtr& operator= (const MatlabHandleForPtr&) = delete;
public:
    ~MatlabHandleForPtr() 
	{ 
		signature_ = 0; 
		delete basePointer_; 
	}
    bool IsValid() 
	{ 
		return ((signature_ == classHandleSignature) && !strcmp(className_.c_str(), typeid(Base).name())); 
	}
    Base* GetBasePointer() 
	{ 
		return basePointer_; 
	}
	operator Base* ()
	{
		return basePointer_;
	}

private:
    uint32_t signature_;
    std::string className_;
    Base* basePointer_;

	friend mxArray* CreateMatlabHandle<Base>(Base *ptr);
};
/// <summary>
/// Takes a pointer, and returns a handle to this pointer in form of a mxArray which can be passed
/// to Matlab. Furthermore, locks the current mex file.
/// The handle must be released lateron by calling DeleteMatlabHandle(), which also calls the destructor of the pointed to object and
/// releases the lock of the mex file.
/// Calling this function passes ownership to the pointed object, i.e. the object must not be deleted otherwise
/// than calling DeleteMatlabHandle().
/// The same object must not be passed more than once to this function.
/// </summary>
/// <param name="ptr">Pointer to an object for which a Matlab handle should be generated.</param>
/// <returns>Matlab handle to the object.</returns>
template<class Base> mxArray* CreateMatlabHandle(Base* ptr)
{
    mexLock();
    mxArray* out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((uint64_t*)mxGetData(out)) = reinterpret_cast<uint64_t>(new MatlabHandleForPtr<Base>(ptr));
    return out;
}
/// <summary>
/// Takes a Matlab handle to an object and returns a pointer to this object.
/// Can be called as often as required.
/// </summary>
/// <param name="in">Matlab handle to an object.</param>
/// <returns>Pointer to the object.</returns>
template<class Base> Base* GetPtrFromMatlabHandle(const mxArray* in)
{
	if (mxGetNumberOfElements(in) != 1 || mxGetClassID(in) != mxUINT64_CLASS || mxIsComplex(in))
		mexErrMsgTxt("Input must be a real uint64 scalar.");
	MatlabHandleForPtr<Base> *ptr = reinterpret_cast<MatlabHandleForPtr<Base>*>(*((uint64_t*)mxGetData(in)));
	if (!ptr->IsValid())
		mexErrMsgTxt("Handle not valid.");
    return ptr->GetBasePointer();
}
/// <summary>
/// Takes a Matlab handle to an object, deletes the object as well as the handle, and releases the lock from the mex file.
/// Must not be called more than once for a given handle, and the handle must afterwards not be used anymore.
/// </summary>
/// <param name="in">Matlab handle which should be released.</param>
template<class Base> void DeleteMatlabHandle(const mxArray *in)
{
	if (mxGetNumberOfElements(in) != 1 || mxGetClassID(in) != mxUINT64_CLASS || mxIsComplex(in))
		mexErrMsgTxt("Input must be a real uint64 scalar.");
	MatlabHandleForPtr<Base> *ptr = reinterpret_cast<MatlabHandleForPtr<Base>*>(*((uint64_t*)mxGetData(in)));
	if (!ptr->IsValid())
		mexErrMsgTxt("Handle not valid.");
    delete ptr;
    mexUnlock();
}
