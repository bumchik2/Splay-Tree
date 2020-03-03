#pragma once

#include <iostream>
#include <stdexcept>

using std::cout;
using std::endl;


bool erasing = false;
int number = 0;


class SplayTree {
public:

	SplayTree() : _root(0) { }
	~SplayTree();

	bool search(int value);
	void insert(int value);
	bool erase(int value);

	bool empty() const;

	void print() const;

	int min();

	int size() const {
		return _root ? _root->size : 0;
	}

	class Node {

	public:

		Node(int value = 0) : value(value), parent(0), left(0), right(0), size(1) { }

		int value;
		Node* parent;
		Node* left;
		Node* right;
		int size;

		void replaceChild (Node* old_child, Node* new_child);

		bool sameDirection ();

	};

	SplayTree(Node* root) : _root(root) { }

private:

	Node* _root;

	void _rightRotate(Node* node);
	void _leftRotate(Node* node);
	void _rotate(Node* node);

	void _zig(Node* node);
	void _zigZig(Node* node);
	void _zigZag(Node* node);

	void _splay(Node* node);

	void _searchValueFromNode(Node* node, int value);

	void _split(Node*& l, Node*& r, int key);
	Node* _merge (Node* tree1, Node* tree2);

	void _makeParent(Node* node, Node* new_parent);
	void _makeRootIfNeeded(Node* node);

	void _destroy(Node* node);

	void _recalc(Node* node);

	Node* _topParent(Node* node) const;

	void _print(Node* node) const;

	static const int INF = static_cast<int>(2e9);

};


void SplayTree::_destroy(SplayTree::Node* node) {
	if (node == 0) {
		return;
	} else {
		_destroy(node->left);
		_destroy(node->right);
		delete node;
	}
}


SplayTree::~SplayTree() {
	_destroy(_root);
}


bool SplayTree::empty () const {
	return _root == 0;
}


void SplayTree::Node::replaceChild (Node* old_child, Node* new_child) {
	if (left == old_child) {
		left = new_child;
	} else if (right == old_child) {
		right = new_child;
	} else {
		throw std::invalid_argument("invalid old_child value in replaceChild");
	}
}


void SplayTree::_makeRootIfNeeded(SplayTree::Node* node) {
	if (node->parent == 0) {
		_root = node;
	}
}


void SplayTree::_rightRotate (SplayTree::Node* node) {

	node->parent->left = node->right;
	if (node->right != 0) {
		node->right->parent = node->parent;
	}
	node->right = node->parent;

	Node* grandParent = node->parent->parent;
	if (grandParent != 0) {
		grandParent->replaceChild(node->parent, node);
	}
	node->parent->parent = node;
	node->parent = grandParent;

	_recalc(node);
	_recalc(node->right);

	_makeRootIfNeeded(node);
}


void SplayTree::_leftRotate (SplayTree::Node* node) {
	node->parent->right = node->left;
	if (node->left != 0) {
		node->left->parent = node->parent;
	}
	node->left = node->parent;

	Node* grandParent = node->parent->parent;
	if (grandParent != 0) {
		grandParent->replaceChild(node->parent, node);
	}
	node->parent->parent = node;
	node->parent = grandParent;

	_recalc(node);
	_recalc(node->left);

	_makeRootIfNeeded(node);
}


void SplayTree::_rotate (SplayTree::Node* node) {
	if (node == node->parent->left) {
		_rightRotate(node);
	} else if (node == node->parent->right) {
		_leftRotate(node);
	} else {
		throw std::runtime_error("impossible scenario");
	}
}


void SplayTree::_zig (SplayTree::Node* node) {
	_rotate(node);
}


void SplayTree::_zigZig (SplayTree::Node* node) {
	_rotate(node->parent);
	_rotate(node);
}


void SplayTree::_zigZag (SplayTree::Node* node) {
	_rotate(node);
	_rotate(node);
}


bool SplayTree::Node::sameDirection() {
	return (this == parent->left && parent == parent->parent->left) ||
			(this == parent->right && parent == parent->parent->right);
}


void SplayTree::_splay (SplayTree::Node* node) {
	if (node == _root) {
		return;
	} else if (node->parent == _root) {
		_zig(node);
	} else if (node->sameDirection() == true) {
		_zigZig(node);
	} else {
		_zigZag(node);
	}
	_splay(node);
}


void SplayTree::_searchValueFromNode(SplayTree::Node* node, int value) {
	if (node == 0) {
		return;
	} else if (node->value == value) {
		_splay(node);
	} else if (node->value > value && node->left != 0) {
		_searchValueFromNode(node->left, value);
	} else if (node->value < value && node->right != 0) {
		_searchValueFromNode(node->right, value);
	} else {
		_splay(node);
	}
}


bool SplayTree::search (int value) {
	_searchValueFromNode(_root, value);
	return _root->value == value;
}


void SplayTree::_makeParent (SplayTree::Node* node, SplayTree::Node* new_parent) {
	if (node != 0) {
		node->parent = new_parent;
	}
}


void SplayTree::_split (SplayTree::Node*& l, SplayTree::Node*& r, int key) {
	_searchValueFromNode(_root, key);
	if (_root->value >= key) {
		r = _root;
		l = _root->left;
		r->left = 0;
		_makeParent(l, 0);
	} else {
		l = _root;
		r = _root->right;
		l->right = 0;
		_makeParent(r, 0);
	}
}


SplayTree::Node* SplayTree::_topParent(Node* node) const {
	if (node == 0) {
		return 0;
	} else {
		while (node->parent != 0) {
			node = node->parent;
		}
		return node;
	}
}


SplayTree::Node* SplayTree::_merge (SplayTree::Node* left, SplayTree::Node* right) {
	if (left == 0 || right == 0) {
		return left ? left : right;
	}
	_searchValueFromNode(left, INF);
	left = _topParent(left);
	left->right = right;
	_recalc(left);
	right->parent = left;
	return left;
}


void SplayTree::insert (int value) {
	if (_root == 0) {
		_root = new Node(value);
	} else {
		_searchValueFromNode(_root, value);
		if (_root->value == value) {
			return;
		} else {
			Node* left = 0;  // < value
			Node* right = 0; // > value
			_split(left, right, value);
			_root = new Node (value);
			_root->left = left;
			_makeParent(left, _root);
			_root->right = right;
			_makeParent(right, _root);
		}
		_recalc(_root);
	}
}


bool SplayTree::erase (int value) {
	erasing = true;
	_searchValueFromNode(_root, value);
	if (_root == 0 || _root->value != value) {
		return false;
	} else {
		_makeParent(_root->left, 0);
		_makeParent(_root->right, 0);
		Node* left = _root->left;
		Node* right = _root->right;
		delete _root;
		_root = _merge(left, right);
		_recalc(_root);
		return true;
	}
}


int getSize(SplayTree::Node* node) {
	return node ? node->size : 0;
}


int SplayTree::min () {
	if (_root == 0) {
		throw std::runtime_error("can't find min in empty tree");
	}
	Node* current_vertex = _root;
	while (current_vertex->left != 0) {
		current_vertex = current_vertex->left;
	}
	_splay(current_vertex);
	return _root->value;
}


void SplayTree::_recalc (SplayTree::Node* node) {
	if (node != 0) {
		node->size = getSize(node->left) + getSize(node->right) + 1;
	}
}


void SplayTree::_print(SplayTree::Node* node) const {
	if (node == 0) {
		return;
	}
	_print(node->left);
	std::cout << node->value << ' ';
	_print(node->right);
}


void SplayTree::print() const {
	_print(_root);
}
