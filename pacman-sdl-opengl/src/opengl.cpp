#include <stdlib.h>

#include "opengl.h"

shader_t::shader_t (GLenum shader_type, const char *fname)
{
	this->shader_type = shader_type;
	this->fname = fname;
	this->shader_id = glCreateShader(this->shader_type);
}

void shader_t::compile ()
{
	// First, read the whole shader file to memory.
	// I usually do this in C, but wanted to do in C++ for a change, but Jesus...

	std::ifstream t(this->fname);
	std::stringstream str_stream;
	str_stream << t.rdbuf();
	std::string buffer = str_stream.str();

	dprint( "loaded shader (" << this->fname << "):" << sdl::endl )
	dprint( buffer )

	exit(0);
	
	const char *c_str = buffer.c_str();
	glShaderSource(this->shader_id, 1, ( const GLchar ** )&c_str, nullptr);
	glCompileShader(this->shader_id);

	GLint status;
	glGetShaderiv(this->shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::cout << "vertex shader compilation failed" << std::endl;
		exit(1);
	}
}

opengl_program_t (shader_t *vs, shader_t *fs)
{
	this->vs = vs;
	this->fs = fs;
	this->program_id = glCreateProgram();
	glAttachShader(this->program_id, this->vs->shader_id);
	glAttachShader(this->program_id, this->fs->shader_id);
}