////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
// Copyright (c) 2023 by Carlos Martinho
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "./mgl/mgl.hpp"

#include <conio.h>
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define P 112

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
	void initCallback(GLFWwindow* win) override;
	void displayCallback(GLFWwindow* win, double elapsed) override;
	void windowSizeCallback(GLFWwindow* win, int width, int height) override;
	void keyCallback(GLFWwindow* window, int key, int scancode, int action,
		int mods) override;
	void scrollCallback(GLFWwindow* window, double xoffset,
		double yoffset) override;
	void mouseButtonCallback(GLFWwindow* window, int button, int action,
		int mods)override;
	//void cursorCallback(GLFWwindow* window, double xpos, double ypos) override;

private:
	const GLuint UBO_BP = 0;
	mgl::ShaderProgram* Shaders = nullptr;
	mgl::Camera* Camera = nullptr;
	GLint ModelMatrixId;
	mgl::Mesh* Mesh = nullptr;
	bool mouseBtnPressed = false;
	double xposl = 0, yposl = 0;
	mgl::SceneGraph* Scene = nullptr;

	void createMeshes();
	void createShaderPrograms();
	void createCamera();
	void createScene();
	void drawScene();
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
	std::string mesh_dir = ".\\assets\\models\\";
	std::string mesh_file = "glass.obj";
	// std::string mesh_file = "cube-vn.obj";
	// std::string mesh_file = "cube-vtn.obj";
	// std::string mesh_file = "cube-vtn-2.obj";
	// std::string mesh_file = "torus-vtn-flat.obj";
	// std::string mesh_file = "torus-vtn-smooth.obj";
	// std::string mesh_file = "suzanne-vtn-flat.obj";
	// std::string mesh_file = "suzanne-vtn-smooth.obj";
	// std::string mesh_file = "teapot-vn-flat.obj";
	// std::string mesh_file = "teapot-vn-smooth.obj";
	// std::string mesh_file = "bunny-vn-flat.obj";
	// std::string mesh_file = "bunny-vn-smooth.obj";
	//std::string mesh_file = "monkey-torus-vtn-flat.obj";
	std::string mesh_fullname = mesh_dir + mesh_file;

	Mesh = new mgl::Mesh();
	Mesh->joinIdenticalVertices();
	Mesh->create(mesh_fullname);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
	Shaders = new mgl::ShaderProgram();
	Shaders->addShader(GL_VERTEX_SHADER, "global-vs.glsl");
	Shaders->addShader(GL_FRAGMENT_SHADER, "global-fs.glsl");

	Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
	if (Mesh->hasNormals()) {
		Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
	}
	if (Mesh->hasTexcoords()) {
		Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
	}
	if (Mesh->hasTangentsAndBitangents()) {
		Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
	}

	Shaders->addUniform(mgl::MODEL_MATRIX);
	Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
	Shaders->addUniform("Time");
	Shaders->addUniform("effect");
	Shaders->create();

	ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA

glm::vec3 eye1 = glm::vec3(-5.0f, 0.0f, 5.0f);
glm::vec3 center = glm::vec3(0.f);
glm::vec3 up1 = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::mat4 ViewMatrix1 =
glm::lookAt(eye1, center,
	up1);

glm::vec3 eye2 = glm::vec3(5.0f, 5.0f, 5.0f);
glm::vec3 up2 = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::mat4 ViewMatrix2 = glm::lookAt(eye2, center, up2);

// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
const glm::mat4 ProjectionMatrix1 =
glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
const glm::mat4 ProjectionMatrix2 =
glm::perspective(glm::radians(30.0f), 640.0f / 480.0f, 1.0f, 10.0f);

void MyApp::createCamera() {
	Camera = new mgl::Camera(UBO_BP);
	mgl::Camera::ViewMatrixInfo vmInfo1{};
	vmInfo1.Eye = eye1;
	vmInfo1.Up = up1;
	vmInfo1.viewMatrix = ViewMatrix1;

	mgl::Camera::ViewMatrixInfo vmInfo2{};
	vmInfo2.Eye = eye2;
	vmInfo2.Up = up2;
	vmInfo2.viewMatrix = ViewMatrix2;
	// TODO doda� zmian� viewMatrix
	Camera->setViewMatrix(ViewMatrix1);
	Camera->setProjectionMatrix(ProjectionMatrix2);
	Camera->addProjectionMatrix(ProjectionMatrix1, mgl::PERSPECTIVE);
	Camera->addProjectionMatrix(ProjectionMatrix2, mgl::ORTHOGRAPHIC);
	Camera->addViewMatrix(vmInfo1, mgl::FROM_Y);
	Camera->addViewMatrix(vmInfo2, mgl::FROM_Z);
}

void MyApp::createScene() {
	std::string mesh_dir = ".\\assets\\models\\";
	std::string glass_file = "glass.obj";
	std::string table_file = "tableColor.obj";
	std::string background_file = "backgroundPlain.obj";
	mgl::Node* sceneRoot = new mgl::Node();

	mgl::Node* tableNode = new mgl::Node();
	mgl::Node* glassNode = new mgl::Node();
	mgl::Node* backgroundPlainNode = new mgl::Node();

	mgl::Mesh* table = new mgl::Mesh();
	table->joinIdenticalVertices();
	table->create(mesh_dir + table_file);
	table->setTransform(nullptr);
	table->setEffect(0);
	tableNode->setParent(sceneRoot);
	tableNode->setMesh(table);
	

	mgl::Mesh* glass = new mgl::Mesh();
	glass->joinIdenticalVertices();
	glass->create(mesh_dir + glass_file);
	glass->setTransform(nullptr);
	glass->setEffect(1);
	glassNode->setParent(tableNode);
	glassNode->setMesh(glass);

	mgl::Mesh* backgroundPlain = new mgl::Mesh();
	mgl::Transform* transform = new mgl::Transform();
	transform->setRotation(180, glm::vec3(0, 1, 0));
	transform->setTranslate(glm::vec3(3, 0, 0));
	transform->calculateModelMatrix();

	backgroundPlain->joinIdenticalVertices();
	backgroundPlain->create(mesh_dir + background_file);
	backgroundPlain->setTransform(transform);

	backgroundPlain->setEffect(2);
	backgroundPlainNode->setParent(sceneRoot);
	backgroundPlainNode->setMesh(backgroundPlain);

	// floor
	mgl::Node* p2Node = new mgl::Node();
	mgl::Mesh* p2 = new mgl::Mesh();
	mgl::Transform* t2 = new mgl::Transform();
	//t2->setRotation(90, glm::vec3(1, 0, 0));
	//t2->setTranslate(glm::vec3(5, 0, 0));
	t2->setScale(glm::vec3(1, 1, 2));

	t2->calculateModelMatrix();
	p2->joinIdenticalVertices();
	p2->create(mesh_dir + "plane.obj");
	p2->setTransform(t2);
	p2->setEffect(3);
	p2Node->setParent(sceneRoot);
	p2Node->setMesh(p2);

	// wall right
	mgl::Node* p3Node = new mgl::Node();
	mgl::Mesh* p3 = new mgl::Mesh();
	mgl::Transform* t3 = new mgl::Transform();
	t3->setRotation(90, glm::vec3(1, 0, 0));
	t3->setTranslate(glm::vec3(0, 0, -2));
	t3->setScale(glm::vec3(1, 1, 2));

	t3->calculateModelMatrix();
	p3->joinIdenticalVertices();
	p3->create(mesh_dir + "plane.obj");
	p3->setTransform(t3);
	p3->setEffect(2);
	p3Node->setParent(sceneRoot);
	p3Node->setMesh(p3);
	Scene = new mgl::SceneGraph();
	Scene->setRoot(sceneRoot);
	Scene->save(".\\scene.json");
	//Scene->load(".\\scene.json");
	//Scene->draw();
}

/////////////////////////////////////////////////////////////////////////// DRAW

glm::mat4 ModelMatrix(1.0f);
void MyApp::drawScene() {
	static double time = 0.0;

	Shaders->bind();
	glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
	Scene->draw(Shaders);
	//Mesh->draw();
	Shaders->unbind();

	time += 0.1;
}


////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
	createMeshes();
	createShaderPrograms();  // after mesh;
	createCamera();
	createScene();
}

void MyApp::windowSizeCallback(GLFWwindow* win, int winx, int winy) {
	glViewport(0, 0, winx, winy);
	// change projection matrices to maintain aspect ratio
}

void MyApp::displayCallback(GLFWwindow* win, double elapsed) { drawScene(); }

void MyApp::keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mods) {

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (Camera->getProjectionType() == mgl::ProjectionType::PERSPECTIVE) {
			Camera->setProjectionType(mgl::ProjectionType::ORTHOGRAPHIC);
		}
		else {
			Camera->setProjectionType(mgl::ProjectionType::PERSPECTIVE);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (Camera->getViewType() == mgl::FROM_Y) {

			Camera->setViewType(mgl::FROM_Z);
		}
		else {
			Camera->setViewType(mgl::FROM_Y);
		}
	}

	glm::vec3 v = glm::normalize(Camera->getViewMatrixInfo().Eye);
	glm::vec3 s = glm::normalize(glm::cross(v, Camera->getViewMatrixInfo().Up));
	glm::vec3 u = glm::normalize(glm::cross(s, v));
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {		
		Camera->rotate(glm::vec3(-1 * u.x, -1 * u.y, -1 * u.z));
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		Camera->rotate(u);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		Camera->rotate(s);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		Camera->rotate(glm::vec3(-1 * s.x, -1 * s.y, -1 * s.z));
	}	
}

void MyApp::mouseButtonCallback(GLFWwindow* window, int button, int action,
	int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseBtnPressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseBtnPressed = false;
	}
}
void MyApp::scrollCallback(GLFWwindow* window, double xoffset,
	double yoffset) {
	if (yoffset > 0) {
		Camera->zoomOut();
	}
	if (yoffset < 0) {
		Camera->zoomIn();
	}
}


/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
	mgl::Engine& engine = mgl::Engine::getInstance();
	engine.setApp(new MyApp());
	engine.setOpenGL(4, 6);
	engine.setWindow(800, 600, "Mesh Loader", 0, 1);
	engine.init();
	engine.run();
	exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
