#include "mglScenegraph.hpp"

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

	void Node::draw() {
		mesh->draw();
		for (Node* n : children) {
			n->draw();
		}
	}

	///////////////////////////////////////////////// SceneGraph
	SceneGraph::SceneGraph() {

	}

	void SceneGraph::setRoot(Node* r) {
		root = r;
	}	

	void SceneGraph::draw() {
		for (Node* n : root->getChildren()) {
			n->draw();
		}
	}

}