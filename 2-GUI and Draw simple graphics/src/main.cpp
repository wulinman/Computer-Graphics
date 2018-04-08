#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
//#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//调整三角形形状以适配窗口
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// input vertices' data（输入顶点信息）
	// ---------------------------------
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.3f, -0.6f, 0.0f, 0.94f, 0.53f, 0.65f,
		 0.3f, -0.6f, 0.0f, 0.65f, 0.33f, 0.98f,
		 0.0f, -0.9f, 0.0f, 0.93f, 0.18f, 0.54f
	};

	unsigned int indices[] = {
		0,1,2,
		3,4,5
	};
	
	// VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 分配空间，传送数据
	// EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// link vertices' attribute（设置顶点属性指针）
	//------------------------------------------
	// vertex位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// vertex shader source
	//----------------------------
	const char *vertexShaderSrc = "#version 440 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 Color;\n"
		"out vec3 vertexColor;\n" // 向片段着色器输出一个颜色
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"	vertexColor = Color;\n"
		"}\0";
	// compile vertex shader
	//----------------------
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); 	//create a shander object
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glCompileShader(vertexShader);
	// 检查vertexShader的编译错误
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader source
	//------------------------------
	const char *fragmentShaderSrc = "#version 440 core\n"
		"in vec3 vertexColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(vertexColor.x, vertexColor.y, vertexColor.z, 1.0f);\n"
		"}\0";
	// compile fragment shader
	//------------------------
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fragmentShader);
	// 检查fragmentShader的编译错误
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// create&link shader programs（链接着色程序）
	//------------------------------------------
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 检查链接错误
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//ImGui Settings
	//--------------
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Initialize some variables
	bool draw_point = false;
	bool draw_line = false;
	bool draw_triangle = true;
	bool draw_2triangle = false;
	ImVec4 newColor = ImVec4(1.0f, 0.5f, 0.2f, 1.00f);

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		// 清除颜色缓冲区，重置为以下颜色
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// glBindVertexArray(0); // no need to unbind it every time 

		// ImGui 设计GUI
		//--------------
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Begin("GUI Window");
		//	ImGui::SetWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
			
			// menu功能1：改变三角形的颜色
			ImGui::Text("Change color of the whole triangle");
			ImGui::ColorEdit3("new color", (float*)&newColor);
			if (ImGui::Button("Set")) {
				for (int i = 0; i < 3; i++) {
					vertices[3 + i * 6] = newColor.x;
					vertices[4 + i * 6] = newColor.y;
					vertices[5 + i * 6] = newColor.z;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				vertices[3] = vertices[10]= vertices[17]= 1.0f;
				vertices[4] = vertices[5]= vertices[9]= vertices[11]= vertices[15]= vertices[16] = 0.0f;
			}
			
			// menu功能2：改变三角形顶点的颜色
			ImGui::Text("Change color of vertices");
			ImGui::ColorEdit3("left vertex", vertices + 3);
			ImGui::ColorEdit3("right vertex", vertices + 9);
			ImGui::ColorEdit3("top vertex", vertices + 15);

			// menu功能3: 绘制点或线
			ImGui::Text("Draw other graphics");
			ImGui::Checkbox("point", &draw_point);
			ImGui::SameLine();
			ImGui::Checkbox("line", &draw_line);
			ImGui::Checkbox("triangle", &draw_triangle);
			ImGui::SameLine();
			
			// menu功能4：使用EBO绘制多个三角形
			ImGui::Checkbox("two triangles", &draw_2triangle);
			ImGui::End();
		}

		// 渲染物体
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		// 重新绑定VBO，将数据送入缓存
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//绘制图形
		if (draw_point) {
			glDrawArrays(GL_POINTS, 0, 3);
			glDrawArrays(GL_POINTS, 3, 3);
			glPointSize(5.0);
		}
		if (draw_line) {
			glDrawArrays(GL_LINES, 3, 2);
			glDrawArrays(GL_LINES, 0, 2);
		}
		if (draw_triangle) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		if (draw_2triangle) {
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		//交换缓存
		glfwSwapBuffers(window); 
		glfwPollEvents();
	}

	// 释放资源
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

// 窗口尺寸改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// 保证视点与新窗口尺寸匹配
	glViewport(0, 0, width, height);
}