#ifndef __sogal_header_lib_h__
#define __sogal_header_lib_h__

#include <math.h>

#define OO_ENCAPSULATE(TYPE, VAR) \
	protected: \
	TYPE VAR; \
	public: \
	inline void set_##VAR (TYPE VAR) { \
		this->VAR = VAR; \
	} \
	inline TYPE get_##VAR () { \
		return this->VAR; \
	}

namespace sogal
{

	inline float fast_sqrt (float v)
	{
		return sqrt(v);
	}

}

#endif