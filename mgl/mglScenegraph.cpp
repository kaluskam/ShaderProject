#include "mglScenegraph.hpp"

#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace mgl {

	Node::Node() {

	}

	Node::~Node() {
		//TODO
	}

	std::vector<Node *> Node::getChildren() {
		return children;
	}

	void Node::setParent(Node* p) {
		parent = p;
		parent->addChild(this);
	}

	void Node::addChild(Node* c) {
		children.push_back(c);
	}

	void Node::setMesh(Mesh* m) {
		mesh = m;
	}

	Mesh* Node::getMesh() {
		return mesh;
	}

	void Node::draw(ShaderProgram* shaderProgram) {
		glUniform1i(shaderProgram->Uniforms["effect"].index, (GLuint) mesh->getEffect());
		mesh->draw();

		for (Node* n : children) {
			n->draw(shaderProgram);
		}
	}

	json Node::toJSON() {
		json j = json({});
		if (mesh != nullptr) {
			j["mesh"] = mesh->toJSON();
		}
		j["children"] = json::array();
		for (int i = 0; i < children.size(); i++) {
			j["children"].push_back(children[i]->toJSON());
		}
		return j;
	}

	void Node::fromJSON(json j) {
		mesh = new Mesh();
		mesh->fromJSON(j["mesh"]);
		for (int i = 0; i < j["children"].size(); i++) {
			Node* child = new Node();
			child->fromJSON(j["children"][i]);
			children.push_back(child);
		}
	}

	///////////////////////////////////////////////// SceneGraph
	SceneGraph::SceneGraph() {

	}

	void SceneGraph::setRoot(Node* r) {
		root = r;
	}	

	void SceneGraph::draw(ShaderProgram* shaderProgram) {
		for (Node* n : root->getChildren()) {
			n->draw(shaderProgram);
		}
	}

	void SceneGraph::save(const char *path) {
		json j = json({});
		j["root"] = root->toJSON();
		std::ofstream o(path);
		o << std::setw(4) << j << std::endl;
		o.close();
	}

	void SceneGraph::load(const char* path) {
		std::ifstream i(path);
		json j;
		i >> j;
		root = new Node();
		for (json::iterator it = j.begin(); it != j.end(); ++it) {
			if (it.key() == "root") {
				root->fromJSON(it.value());
			}
		}
		
	}
}