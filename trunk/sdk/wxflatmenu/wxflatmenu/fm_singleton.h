///////////////////////////////////////////////////////////////////////////////
// Name:		singleton.h
// Purpose:     A template class that implements the fmSingleton pattern 
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef CODELITE_SINGLETON_H
#define CODELITE_SINGLETON_H
/**
 * A template class that implements the fmSingleton pattern.
 *
 * \date 08-23-2006
 * \author eran
 */
template <typename T>
class fmSingleton
{
	static T* ms_instance;
public:
	/**
	 * Static method to access the only pointer of this instance.
	 * \return a pointer to the only instance of this 
	 */
	static T* Get();

	/**
	 * Release resources.
	 */
	static void Free();

protected:
	/**
	 * Default constructor.
	 */
	fmSingleton();

	/**
	 * Destructor.
	 */
	virtual ~fmSingleton();
};
template <typename T>
T* fmSingleton<T>::ms_instance = 0;

template <typename T>
fmSingleton<T>::fmSingleton()
{
}

template <typename T>
fmSingleton<T>::~fmSingleton()
{
}

template <typename T>
T* fmSingleton<T>::Get()
{
	if(!ms_instance)
		ms_instance = new T();
	return ms_instance;
}

template <typename T>
void fmSingleton<T>::Free()
{
	if( ms_instance )
	{
		delete ms_instance;
		ms_instance = 0;
	}
}

#endif // CODELITE_SINGLETON_H
