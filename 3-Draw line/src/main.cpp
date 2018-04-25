#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
//#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector> 

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
vector<int> bresenham(int x1, int y1, int x2, int y2);
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void drawCircle(int x0, int y0, int r);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int window_w = 800;
int window_h = 600;


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
	// 初始化三角形三个点的位置
	int xPos[] = { -200,0,200 };
	int yPos[] = { 0,300,0 };

	// 初始化圆形圆心、半径
	int x0 = 0, y0 = 0, r = 100;
	int flag = 0;
	

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &window_w, &window_h);
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
			//ImGui::Text("Draw Triangle");
			//ImGui::ColorEdit3("new color", &newColor);
			if (ImGui::Button("Draw Triangle")) {
				flag = 0;
			}

			ImGui::InputInt("vertex1.x", xPos);
			ImGui::InputInt("vertex1.y", yPos);

			ImGui::InputInt("vertex2.x", (xPos + 1));
			ImGui::InputInt("vertex2.y", (yPos + 1));

			ImGui::InputInt("vertex3.x", (xPos + 2));
			ImGui::InputInt("vertex3.y", (yPos + 2));
			ImGui::NewLine();
			if (ImGui::Button("Draw Circle")) {
				flag = 1;
			}
			ImGui::InputInt("center.x", &x0);
			ImGui::InputInt("center.y", &y0);
			ImGui::InputInt("radius", &r);

			ImGui::End(); 
		}

		// 渲染物体
		glUseProgram(shaderProgram);
		if (flag == 0) {
			drawTriangle(xPos[0], yPos[0], xPos[1], yPos[1], xPos[2], yPos[2]);
		}
		else if (flag == 1) {
			drawCircle(x0,y0, r);
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
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

// 窗口尺寸改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// 保证视点与新窗口尺寸匹配
	glViewport(0, 0, width, height);
}

vector<int> bresenham(int x1, int y1, int x2, int y2) {
	vector<int> points;
	// 计算dx, dy, p0
	int dx, dy, p;
	int flag = 0;
	// 当前x,y
	int x, y;
	float k;

	// 计算斜率K
	if (x1 != x2) {

		// 确保左边点为x1,y1
		if (x1 > x2) {
			swap(x1, x2);
			swap(y1, y2);
		}
		points.push_back(x1);
		points.push_back(y1);

		k = (float)(y2 - y1) / (x2 - x1);
		//cout << "k" << k << endl;

		if (k < 0 && k >= -1) {
			flag = 1;
			y1 = -y1; // 作关于x轴的对称直线
			y2 = -y2;
		}
		//对称轴 y=x
		if (k > 1) {
			flag = 2;
			swap(x1, y1);
			swap(x2, y2);
		}
		if (k < -1) {
			flag = 3;
			y1 = -y1;
			y2 = -y2;
			swap(x1, y1);
			swap(x2, y2);
		}
	}
	if (x1 == x2) { // k不存在,垂直线
		flag = 4;
		swap(x1, y1);
		swap(x2, y2);
		
		if (x1 > x2) {
			swap(x1, x2);
			swap(y1, y2);
		}
		points.push_back(y1);
		points.push_back(x1);
	}

	dx = abs(x1 - x2);
	dy = abs(y1 - y2);
	x = x1, y = y1;
	p = 2 * dy - dx;

	for (int i = 0; x < x2; i++) {
		if (p <= 0) {
			x = x++;
			y = y;
			p = p + 2 * dy;
		}
		if (p > 0) {
			x = x++;
			y = y++;
			p = p + 2 * dy - 2 * dx;
		}
		if (flag == 0) {
			points.push_back(x);
			points.push_back(y);
		}
		if (flag == 1) {
			points.push_back(x);
			points.push_back(-y); 
		}
		if (flag == 2 || flag == 4) {
			points.push_back(y);
			points.push_back(x);
		}
		if (flag == 3) {
			points.push_back(y);
			points.push_back(-x);
		}
	}

	return points;
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {

	// 计算得到三条直线的绘制点
	vector<int> line1 = bresenham(x1, y1, x2, y2);
	//cout << "line1\n" << endl;
	vector<int> line2 = bresenham(x1, y1, x3, y3);
	//cout << "line2\n" << endl;
	vector<int> line3 = bresenham(x3, y3, x2, y2);
	//cout << "line3\n" << endl;

	for (unsigned int i = 0; i < line2.size(); i++) {
		line1.push_back(line2[i]);
	}
	for (unsigned int i = 0; i < line3.size(); i++) {
		line1.push_back(line3[i]);
	}

	// 计算所有点的个数
	int pointNum = line1.size() / 2;

	float* vertices = new float[pointNum * 6];
	for (int i = 0; i < pointNum; i++) {
		// 添加位置值
		vertices[6 * i] = (float)line1[2 * i] / window_w;
		vertices[6 * i + 1] = (float)line1[2 * i + 1] / window_h;
		vertices[6 * i + 2] = 0.0f;
		// 添加颜色值
		vertices[6 * i + 3] = 1.0f;
		vertices[6 * i + 4] = 0.5f;
		vertices[6 * i + 5] = 0.2f;
	}

	// VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*pointNum * 6, vertices, GL_STATIC_DRAW); // 分配空间，传送数据

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);
	// 重新绑定VBO，将数据送入缓存
	/*glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*pointNum * 6, vertices, GL_STATIC_DRAW);*/

	glDrawArrays(GL_POINTS, 0, pointNum);
	glPointSize(2.0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete[] vertices;
}

void drawCircle(int x0, int y0, int r) {

	int x = 0, y = r; 	// 从（0，r)开始画

	// 计算下一组可取点的中点到圆心的距离
	int p = 5-4*r;
	vector<int> points;
	for (int i = 0; x <= y; i++) {
		if (p < 0) {
			p = p + 8 * x + 12;
		}
		else {
			p = p + 8 * (x - y) + 20;
			y--;
		}
		x++;

		// 8个点
		points.push_back(x0 + x), points.push_back(y0 + y);
		points.push_back(x0 + y), points.push_back(y0 + x);
		points.push_back(x0 - x), points.push_back(y0 + y);
		points.push_back(x0 + x), points.push_back(y0 - y);
		points.push_back(x0 + y), points.push_back(y0 - x);
		points.push_back(x0 - y), points.push_back(y0 + x);
		points.push_back(x0 - x), points.push_back(y0 - y);
		points.push_back(x0 - y), points.push_back(y0 - x);
	}

	int pointNum = points.size() / 2;
	float* vertices = new float[pointNum * 6];
	for (int i = 0; i < pointNum; i++) {
		vertices[6 * i] = (float)points[2 * i] / window_w;
		vertices[6 * i + 1] = (float)points[2 * i + 1] / window_h;
		vertices[6 * i + 2] = 0.0f;
		vertices[6 * i + 3] = 1.0f;
		vertices[6 * i + 4] = 0.5f;
		vertices[6 * i + 5] = 0.2f;
	}
	// VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*pointNum * 6, vertices, GL_STATIC_DRAW); // 分配空间，传送数据

    // vertex位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_POINTS, 0, pointNum);
	glPointSize(2.0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete[] vertices;
}

