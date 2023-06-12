#ifndef __PACMAN_SDL_OPENGL_LIB_HEADER_H__
#define __PACMAN_SDL_OPENGL_LIB_HEADER_H__

#include <iostream>
#include <string>

// ---------------------------------------------------

#define DEBUG

// ---------------------------------------------------

#define blikely(x)       __builtin_expect((x),1)
#define bunlikely(x)     __builtin_expect((x),0)

// ---------------------------------------------------

#ifdef DEBUG
	#define dprint(STR) { std::cout << STR; }
#else
	#define dprint(STR)
#endif

// ---------------------------------------------------

#define OO_ENCAPSULATE(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE& VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE& get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define OO_ENCAPSULATE_REFERENCE_READONLY(TYPE, VAR) \
	protected: \
		TYPE VAR; \
	public: \
		inline TYPE& get_##VAR () { \
			return this->VAR; \
		} \
	protected:

#define ASSERT(V) ASSERT_PRINT(V, "bye!\n")

#define ASSERT_PRINT(V, STR) \
	if (bunlikely(!(V))) { \
		std::string assert_str_ = (STR); \
		std::cout << "sanity error!" << std::endl << "file " << __FILE__ << " at line " << __LINE__ << " assertion failed!" << std::endl << #V << std::endl; \
		std::cout << assert_str_ << std::endl; \
		exit(1); \
	}

// ---------------------------------------------------

template<typename T, int w, int h>
class static_matrix_t
{
private:
	T storage[h * w];

public:
	inline T* get_raw ()
	{
		return this->storage;
	}

	inline T& operator() (int j, int i)
	{
		return this->storage[j*w + i];
	}
};

#endif