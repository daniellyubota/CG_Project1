#pragma once

class Shader
{
public:
	GLuint ID;

	Shader(const char* vert_path, const char* frag_path)
	{
		char character;

		std::ifstream vert_stream;
		std::ifstream frag_stream;

		std::string vert_string;
		std::string frag_string;

		vert_stream.open(vert_path);

		if (vert_stream.is_open())
		{
			while (vert_stream.get(character))
				vert_string += character;

			vert_stream.close();
			std::cout << "File: " << vert_path << " opened successfully.\n\n";
		}
		else
			std::cout << "ERROR!... File: " << vert_path << " could not be opened.\n\n";

		frag_stream.open(frag_path);

		if (frag_stream.is_open())
		{
			while (frag_stream.get(character))
				frag_string += character;

			frag_stream.close();
			std::cout << "File: " << frag_path << " opened successfully.\n\n";
		}
		else
			std::cout << "ERROR!... File: " << frag_path << " could not be opened.\n\n";

		std::cout << vert_string << "\n\n";
		std::cout << frag_string << "\n\n";

		const char* vert_pointer = vert_string.c_str();
		const char* frag_pointer = frag_string.c_str();

		GLuint vert_shad, frag_shad;

		vert_shad = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_shad, 1, &vert_pointer, NULL);
		glCompileShader(vert_shad);
		Check_Shaders_Program(vert_shad, "vert_shader");

		frag_shad = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_shad, 1, &frag_pointer, NULL);
		glCompileShader(frag_shad);
		Check_Shaders_Program(frag_shad, "frag_shader");

		ID = glCreateProgram();
		glAttachShader(ID, vert_shad);
		glAttachShader(ID, frag_shad);
		glLinkProgram(ID);
		Check_Shaders_Program(ID, "shader_program");

		glDeleteShader(vert_shad);
		glDeleteShader(frag_shad);
	}

	void use()
	{
		glUseProgram(ID);
	}

private:
	void Check_Shaders_Program(GLuint type, std::string name)
	{
		int success;
		int error_log_size;
		char info_log[1000]; 

		if (name == "vert_shader" || name == "frag_shader")
		{
			glGetShaderiv(type, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(type, 1024, &error_log_size, info_log);
				std::cout << "\n--- Shader Compilation Error: " << name << "\n\n" << info_log << "\n" << "Error Log Number of Characters: " << error_log_size << "\n\n";
			}
		}
		else 
		{
			glGetProgramiv(type, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(type, 1024, &error_log_size, info_log);
				std::cout << "\n--- Program Link Error: " << name << "\n\n" << info_log << "\n" << "Error Log Number of Characters: " << error_log_size << "\n";
			}
		}
	}
};
