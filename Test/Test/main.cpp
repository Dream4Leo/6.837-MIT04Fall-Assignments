#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <camera.h>
#include <shader_s.h>

#include <iostream>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

void updateLineVAOandVBO(int id);
void normalize(double &x, double &y);
int selectVertex(double x, double y);

// settings
const unsigned int SCR_WIDTH	= 1000;
const unsigned int SCR_HEIGHT	= 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// infomation
double lastX = SCR_WIDTH  / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstclick = false;
int vertexID = -1;
int tesllate = 100;

// data
vector<unsigned int> lineVAOs, lineVBOs;
unsigned int bezierVAO, bezierVBO;
vector<double> vertices;	// clip space coords {x0,y0,x1,y1...xn-1,yn-1}
int vertexnum; // = vertices.size() / 2 = lineVAOs.size()

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Curves Test", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	glPointSize(15.0f);
	glEnable(GL_DEPTH_TEST);

	//create shader
	Shader line_shader("line.vs", "line.fs");
	Shader bezier_shader("curve.vs", "curve.fs");

	//load vertex coords
	vertexnum = 7;
	vertices.clear();
	vertices.push_back( 0.5);	vertices.push_back( 0.5);
	vertices.push_back(-0.5);	vertices.push_back( 0.5);
	vertices.push_back( 0.5);	vertices.push_back(-0.5);
	vertices.push_back(-0.5);	vertices.push_back(-0.5); // already in clip space
	vertices.push_back(-0.6);	vertices.push_back( 0.5);
	vertices.push_back(-0.6);	vertices.push_back(-0.5);
	vertices.push_back(-0.5);	vertices.push_back(-0.6);

	// initialize line
	for (int i = 0; i < vertexnum - 1; ++i)
	{
		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		lineVAOs.push_back(VAO);
		lineVBOs.push_back(VBO);
		updateLineVAOandVBO(i);
	}
	// initialize bezier
	float *param = new float[tesllate+1];
	for (int i = 0; i <= tesllate; ++i) param[i] = i * 1.0 / tesllate;
	glGenVertexArrays(1, &bezierVAO);
	glGenBuffers(1, &bezierVBO);
	// Bind
	glBindVertexArray(bezierVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
	glBufferData(GL_ARRAY_BUFFER, (tesllate+1) * sizeof(float), param, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(8.0f);
		bezier_shader.use();
		bezier_shader.setMat4("bezier",
			glm::mat4(-1., 3.,-3., 1.,
				         3.,-6., 3., 0.,
				        -3., 3., 0., 0.,
				         1., 0., 0., 0.));
		glBindVertexArray(bezierVAO);
		for (int i = 0; i < vertexnum - 1; i += 3)
		{
			bezier_shader.setVec2("v0", glm::vec2(vertices[i*2  ], vertices[i*2+1]));
			bezier_shader.setVec2("v1", glm::vec2(vertices[i*2+2], vertices[i*2+3]));
			bezier_shader.setVec2("v2", glm::vec2(vertices[i*2+4], vertices[i*2+5]));
			bezier_shader.setVec2("v3", glm::vec2(vertices[i*2+6], vertices[i*2+7]));
			glDrawArrays(GL_LINE_STRIP, 0, tesllate+1);
		}

		line_shader.use();
		glLineWidth(5.0f);
		for (vector<unsigned int>::iterator iter = lineVAOs.begin(); iter != lineVAOs.end(); ++iter)
		{
			glBindVertexArray(*iter);
			glDrawArrays(GL_POINTS, 0, 2);
			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(lineVAOs.size(), lineVAOs.data());
	glDeleteBuffers(lineVBOs.size(), lineVBOs.data());
	glfwTerminate();
	return 0;
}

void updateLineVAOandVBO(int id)
{
	double newvertices[] = {vertices[id*2  ], vertices[id*2+1], 
													vertices[id*2+2], vertices[id*2+3]};
	//Bind
	glBindVertexArray(lineVAOs[id]);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBOs[id]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(double), newvertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);
	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (vertexID == -1) return;
	normalize(xpos, ypos);
	vertices[vertexID<<1]   = xpos;
	vertices[vertexID<<1|1] = ypos;
	if (vertexID < vertexnum - 1) updateLineVAOandVBO(vertexID);
	if (vertexID > 0)             updateLineVAOandVBO(vertexID-1);

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT) return;
	if (action == GLFW_PRESS)
	{ //Detect onClick vertex
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		normalize(x, y); // screen space back to clip space
		vertexID = selectVertex(x, y);
	}
	else
	{
		vertexID = -1;
	}
}
void normalize(double &x, double &y)
{
	x = x / SCR_WIDTH * 2 - 1.0;
	y = 1.0 - y / SCR_HEIGHT * 2;
}
int selectVertex(double x, double y)
{
	for (int i = 0; i < vertexnum; ++i)
		if (abs(vertices[i<<1] - x) + abs(vertices[i<<1|1] - y) <= 1e-1) return i;
	return -1;
}