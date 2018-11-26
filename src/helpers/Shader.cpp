#include "Shader.h"

Shader::Shader(std::string sourcePath, GLenum shaderType) {

	id = glCreateShader(shaderType);

	if (id < 0) {
		std::cout << "Shader creation failed!" << std::endl;
		std::cout << "OpenGl error:" << glGetError() << std::endl;
        std::exit(-1);
	}

	// read in shader file

	std::ifstream file = std::ifstream(sourcePath, std::ios::in);

    if (!file) {
        std::cout << "Could not open shader source: " << sourcePath << std::endl;
        std::exit(-1);
    }

	std::stringstream ss;
	ss << file.rdbuf();

    file.close();

    std::string sourceString = ss.str();
	const GLchar* sourceCode = sourceString.c_str();

	glShaderSource(id, 1, &sourceCode, 0);
	glCompileShader(id);

	// pull compilation result

	int compileState;
	glGetShaderiv(id, GL_COMPILE_STATUS, &compileState);

	if (compileState == GL_FALSE) {
		// error handling, pull and print error log

		int logSize;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);

		char logData[logSize];
		glGetShaderInfoLog(id, logSize, &logSize, logData);

		glDeleteShader(id);

		std::cout << "Shader compilation failed for: " << sourcePath << std::endl;
		std::cout << logData << std::endl;
        std::exit(-1);
	}

}

Shader::~Shader() {
    
    glDeleteShader(id);
}
