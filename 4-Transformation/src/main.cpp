#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void drawCircle(int x0, int y0, int r);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
int WIN_WIDTH;
int WIN_HEIGHT;

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
	float vertices[] = {
		-0.2f, -0.2f,  0.2f, 1.0f, 0.5f, 0.2f,
		-0.2f,  0.2f,  0.2f, 0.9f, 0.4f, 0.3f,
		 0.2f, -0.2f,  0.2f, 0.8f, 0.3f, 0.4f,
		 0.2f,  0.2f,  0.2f, 0.7f, 0.2f, 0.5f,
		 0.2f, -0.2f, -0.2f, 0.6f, 0.1f, 0.6f,
		 0.2f,  0.2f, -0.2f, 0.5f, 0.6f, 0.7f,
		-0.2f, -0.2f, -0.2f, 0.4f, 0.7f, 0.8f,
		-0.2f,  0.2f, -0.2f, 0.3f, 0.8f, 0.9f,
	};

	unsigned int indices[] = {
		0,1,2,
		1,2,3,
		2,3,4,
		3,4,5,
		4,5,6,
		5,6,7,
		0,6,7,
		0,1,7,
		1,5,7,
		1,3,5,
		0,2,4,
		0,4,6,
	};
	glm::vec3 cubePos[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(5.0f, 0.0f, -5.0f)
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
	// vertex位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// vertex颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader myShader("shader.vs", "shader.fs");
	Shader myShader2("shader2.vs", "shader.fs");

	//ImGui Settings
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Initialize some variables
	int flag= 0;
	bool translate = true;
	bool rotate = true;
	bool scale = true;

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &WIN_WIDTH, &WIN_HEIGHT);
		// 清除颜色缓冲区，重置为以下颜色
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		// 激活深度测试
		glEnable(GL_DEPTH_TEST);
		// glBindVertexArray(0); // no need to unbind it every time 

		// ImGui 设计GUI
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Begin("GUI Window");
			ImGui::RadioButton("Basic", &flag, 0);
			ImGui::Text("Select transformations:");
			ImGui::Checkbox("Translate", &translate);
			ImGui::Checkbox("Rotate", &rotate);
			ImGui::Checkbox("Scale", &scale);
			ImGui::NewLine();
			ImGui::RadioButton("Bonus", &flag, 1);

			ImGui::End();
		}

		// Initialize some transform matrixs
		glm::mat4 trans;
		glm::mat4 view;
		glm::mat4 projection;
		unsigned int transformLoc;
		
		if (flag == 0) {
			// 渲染物体
			myShader.use();
			
			// Basic-2
			if (translate) {
				trans = glm::translate(trans, glm::vec3(0.0f, 0.6*sin(0.5*(float)glfwGetTime()), 0.0f));
			}

			// Basic-3
			if (rotate) {
				trans = glm::rotate(trans, 10 * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));
			}

			// Basic-4
			if (scale) {
				float scaleFactor = abs(sin((float)glfwGetTime())) + 0.5;
				trans = glm::scale(trans, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
			}

			// 将transform传给shader中的uniform变量
			unsigned int transformLoc = glGetUniformLocation(myShader.ID, "trans");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

			// 绘制图元
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		// Bonus
		if (flag == 1) {
			myShader2.use();	
			projection = glm::perspective(45.0f, (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -25.0f));

			unsigned int transformLoc = glGetUniformLocation(myShader2.ID, "projection");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));
			transformLoc = glGetUniformLocation(myShader2.ID, "view");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(view));
			//myShader2.setMat4("projection", projection);
			//myShader2.setMat4("view", view);
			for (unsigned int i = 0; i < 2; i++) {
				glm::mat4 model;
				if (i == 0) { // 太阳
					model = glm::rotate(model, 10 * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f)); // 自转
					model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
				}
				if (i == 1) { // 地球
					model = glm::rotate(model, 25 * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)); // 公转
					model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
				}
				model = glm::translate(model, cubePos[i]);
				float angle = 20.0f*i;
				model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

				unsigned int transformLoc = glGetUniformLocation(myShader2.ID, "model");
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
			// 绘制行星轨道
			myShader.use();
			glm::mat4 transCircle;
			transCircle = glm::scale(transCircle, glm::vec3(0.8f, 0.15f, 0.8f));
			transformLoc = glGetUniformLocation(myShader.ID, "trans");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transCircle));
			drawCircle(0, 0, 1000);
			drawCircle(0, 0, 800);
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

void drawCircle(int x0, int y0, int r) {

	int x = 0, y = r; 	// 从（0，r)开始画

	// 计算下一组可取点的中点到圆心的距离
	int p = 5 - 4 * r;
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
		vertices[6 * i] = (float)points[2 * i] / WIN_WIDTH;
		vertices[6 * i + 1] = (float)points[2 * i + 1] / WIN_HEIGHT;
		vertices[6 * i + 2] = 0.0f;
		vertices[6 * i + 3] = 0.5f;
		vertices[6 * i + 4] = 0.5f;
		vertices[6 * i + 5] = 0.5f;
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