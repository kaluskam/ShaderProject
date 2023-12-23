////////////////////////////////////////////////////////////////////////////////
//
// Scene Management Class
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_SCENEGRAPH_HPP
#define MGL_SCENEGRAPH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <json.hpp>

using json = nlohmann::json;

#include "mglMesh.hpp"
#include "mglShader.hpp"

namespace mgl {

class Node {
private:
	static std::vector<Node*> nodes;
	Mesh *mesh = nullptr;
protected:
	Node *parent = nullptr;
	std::vector<Node *> children;
public:
	static std::vector<Node *> &getNodes(void);
	std::vector<Node *> getChildren();
	void addChild(Node *);
	void setParent(Node *);
	Node &getParent(void);
	Node();
	virtual ~Node();
	void setMesh(Mesh* m);
	Mesh* getMesh();
	void draw(ShaderProgram*);
	json toJSON();
	void fromJSON(json j);

};

class SceneGraph {
private:
	Node* root;
public:
	SceneGraph();
	~SceneGraph();
	void addNode(Node *node);
	void draw(ShaderProgram*);
	void load(const char* path);
	void save(const char* path);
	void setRoot(Node *node);
	Node &getRoot();
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_SCENEGRAPH_HPP */
