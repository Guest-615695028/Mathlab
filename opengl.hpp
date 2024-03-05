#ifndef __ANGEL_H__
#define __ANGEL_H__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "math.hpp"
#include "linearlist.hpp"
#include "matrix.hpp"

#define BUFFER_OFFSET( offset ) ((void*)(offset))
// Defined constant for when numbers are too small to be used in the
// denominator of a division operation.  This is only used if the
// DEBUG macro is defined.
const GLfloat DIV0_TOL = 0x1.0p-16f;
// Degrees-to-radians constant 
const GLfloat DEG_TO_RAD = M_PI / 180.0;

namespace Mathlab {
	// 1 General Initialization
	// 1.1 Create a GLSL program object from vertex and fragment shader files
	template <bool X = false>
	GLuint initShader(const char* vshader, const char* fshader) {
		struct Shader {
			const char* filename;
			GLenum type;
		} shaders[2] = {
			{vshader, GL_VERTEX_SHADER},
			{fshader, GL_FRAGMENT_SHADER}
		};
		GLuint program = glCreateProgram();
		if (!program) return 0;
		for (Shader s : shaders) {
			GLint shader = glCreateShader(s.type), n = 0;
			if constexpr (X) {
				FILE* fp = fopen(s.filename, "r");
				if (!fp) {
					throw Error(ENOENT, "Failed to read %s\n", s.filename);
				}
				fseek(fp, 0, SEEK_END);
				n = ftell(fp) + 1;
				fseek(fp, 0, SEEK_SET);
				char* text = new char[n] {0};
				fread(text, 1, n, fp);
				fclose(fp);
				glShaderSource(shader, 1, &text, NULL);
				delete[] text;
			} else {
				glShaderSource(shader, 1, &s.filename, NULL);
			}
			glCompileShader(shader);
			GLint compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (!compiled) {
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &n);
				char* text = new char[n] {0};
				glGetShaderInfoLog(shader, n, NULL, text);
				Error e(glGetError(), "%s failed to compile:\n%s\n", s.filename, text);
				delete[] text;
				throw e;
			}
			glAttachShader(program, shader);
		}
		// link and error check
		glLinkProgram(program);
		GLint linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked) {
			glUseProgram(program);
		} else {
			GLint n = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &n);
			char* text = new char[n] {0};
			glGetProgramInfoLog(program, n, NULL, text);
			Error e(glGetError(), "Shader program failed to link:\n%s\n", text);
			delete[] text;
			throw e;
		}
		return program;
	}
	GLuint initShaderSource(const char* vshader, const char* fshader) {
		struct Shader {
			const char* filename;
			GLenum type;
		} shaders[2] = {
			{vshader, GL_VERTEX_SHADER},
			{fshader, GL_FRAGMENT_SHADER}
		};
		GLuint program = glCreateProgram();
		if (!program) return 0;
		for (Shader s : shaders) {
			GLuint shader = glCreateShader(s.type);
			glShaderSource(shader, 1, &s.filename, NULL);
			glCompileShader(shader);
			GLint compiled = 0, n = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (!compiled) {
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &n);
				char* text = new char[n] {0};
				glGetShaderInfoLog(shader, n, NULL, text);
				Error e(glGetError(), "%s failed to compile:\n%s\n", s.filename, text);
				delete[] text;
				throw e;
			}
			glAttachShader(program, shader);
		}
	}
	// 1.2 Initialize OpenGL window, called first in main(), then initShader()
	GLFWwindow* initWindow(unsigned int ver = 0x0400, unsigned int width = 600, unsigned int height = 600,
		const char* title = 0,
		GLFWkeyfun keyCallback = 0,
		GLFWframebuffersizefun framebufferSizeCallback = 0) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver >> 8u & 0xFF);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver & 0xFF);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (!window) {
			glfwTerminate();
			throw Error(glGetError(), "Failed to create GLFW window");
		}
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			glfwTerminate();
			throw Error(glGetError(), "Failed to initialize GLAD");
		}
		glClearColor(0, 0, 0, 0);
		glEnable(GL_DEPTH_TEST);
		return window;
	}
	// 2 Debugger
	int debug(GLint program) {
		int n = printf("Error Code: %d\n", glGetError());
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &n);
		if (n <= 0) return 0;
		char* text = new char[n] {0};
		glGetProgramInfoLog(program, n, NULL, text);
		n = puts(text);
		delete[] text;
		return n;
	}
	// 3 Mesh
	class Mesh {
		GLuint _vao, _vbo[16];
		GLint _program;
		LinearList<Vector<float, 4>> _pos, _col, _norm;
	public:
		typedef LinearList<Vector<float, 4>> PointType;
		static constexpr int UNIT_SIZE = sizeof(Vector<float, 4>);
		constexpr Mesh(const PointType& pos,
			const PointType& col, const PointType& norm)
			: _program(1), _vao(0), _vbo{0}, _pos(pos), _col(col), _norm(norm) {
			glGetIntegerv(GL_CURRENT_PROGRAM, &_program);
			glGenVertexArrays(1, &_vao);
			glGenBuffers(2, _vbo);
			glBindVertexArray(_vao);
			glLinkProgram(_program);
		}
		constexpr ~Mesh() {}
		void draw(GLenum usage = GL_STATIC_DRAW) {
			glBindVertexArray(_vao);
			// Position
			GLint posAttrib = glGetAttribLocation(_program, "position");
			glEnableVertexAttribArray(posAttrib);
			GLint posSize = _pos.size() * UNIT_SIZE;
			glVertexAttribPointer(posAttrib, 4, GL_FLOAT, 0, posSize, 0);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, posSize, _pos.data(), usage);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Color
			GLint colAttrib = glGetAttribLocation(_program, "color");
			glEnableVertexAttribArray(colAttrib);
			GLint colSize = _col.size() * UNIT_SIZE;
			glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, colSize, 0);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, _col.size() * UNIT_SIZE, _col.data(), usage);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindVertexArray(_vao);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
			static int i = Mathlab::debug(_program);
		}
	};
}
#endif