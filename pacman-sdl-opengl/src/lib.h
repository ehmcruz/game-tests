#ifndef __PACMAN_SDL_OPENGL_LIB_HEADER_H__
#define __PACMAN_SDL_OPENGL_LIB_HEADER_H__

#define DEBUG

#define blikely(x)       __builtin_expect((x),1)
#define bunlikely(x)     __builtin_expect((x),0)

#ifdef DEBUG
	#define dprint(STR) { std::cout << STR; }
#else
	#define dprint(STR)
#endif

#define OO_ENCAPSULATE(TYPE, VAR) \
	private: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE get_##VAR () { \
			return this->VAR; \
		}

#define OO_ENCAPSULATE_REFERENCE(TYPE, VAR) \
	private: \
		TYPE VAR; \
	public: \
		inline void set_##VAR (TYPE& VAR) { \
			this->VAR = VAR; \
		} \
		inline TYPE& get_##VAR () { \
			return this->VAR; \
		}

#define ASSERT(V) C_ASSERT_PRINT(V, "bye!\n")

#define C_ASSERT_PRINT(V, STR) \
	if (bunlikely(!(V))) { \
		std::string assert_str_ = (STR); \
		std::cout << "sanity error!" << std::endl << "file " << __FILE__ << " at line " << __LINE__ << " assertion failed!" << std::endl << #V << std::endl; \
		std::cout << assert_str_ << std::endl; \
		exit(1); \
	}

#endif