#include "ShaderProgram.h"

#include <iostream>

ShaderProgram::ShaderProgram(GLuint vertexShaderId, GLuint fragShaderId) {

    id = glCreateProgram();

	if (id == 0) {
		std::cout << "Program creation failed!" << std::endl;
		std::cout << "OpenGl error:" << glGetError() << std::endl;
        std::exit(-1);
	}

    glAttachShader(id, vertexShaderId);
	glAttachShader(id, fragShaderId);

	glLinkProgram(id);

	int linkState;

	glGetProgramiv(id, GL_LINK_STATUS, &linkState);

	if (linkState == GL_FALSE) {
		// error handling, pull and print error log

		int logSize;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);

		char* logData = new char[logSize];
		glGetProgramInfoLog(id, logSize, &logSize, logData);

		std::cout << "Program linking failed!" << std::endl;
		std::cout << logData << std::endl;
		
        glDeleteProgram(id);

        delete[] logData;

        std::exit(-1);
	}
}

ShaderProgram::ShaderProgram(Shader vertexShader, Shader fragShader)
    : ShaderProgram(vertexShader.id, fragShader.id) {
}

ShaderProgram::~ShaderProgram() {

    glDeleteProgram(id);
}
