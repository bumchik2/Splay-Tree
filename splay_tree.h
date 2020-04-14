#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>


class SplayTree {
public:

  SplayTree() : _root(0) { }
  ~SplayTree();

  void insert(long long value, int position);
  bool erase(int key);

  int size() const;
  bool empty() const;

  void print(bool detailed = false);

  long long min();

  void makeRotation (int l, int r);

  long long sum (int l, int r);
  void assign (int l, int r, long long value);
  void add (int l, int r, long long delta);
  void next_permutation(int l, int r);
  void prev_permutation(int l, int r);

  void processRequests();

private:

  class Node {
  public:

    Node(long long value) : value(value), parent(0), left(0), right(0), size(1), sum(value),
        leftest(value), rightest(value) { }

    long long value;
    Node* parent;
    Node* left;
    Node* right;
    int size;
    long long sum;

    long long leftest;
    long long rightest;

    bool reversed = false;

    int increasing_prefix = 1;
    int increasing_suffix = 1;
    int decreasing_prefix = 1;
    int decreasing_suffix = 1;

    bool is_assigned = false;
    long long assigned_value = 0;
    bool is_added = false;
    long long added_value = 0;

    bool sameDirection ();
	};

private:

  Node* _root;

  void _push(Node* node);

  void _updateDP (Node* node);
  long long _getValue (Node* node);
  long long _getSum (Node* node);
  int _getSize (Node* node);

  int _getIncreasingPrefix (Node* node);
  int _getIncreasingSuffix (Node* node);
  int _getDecreasingPrefix (Node* node);
  int _getDecreasingSuffix (Node* node);

  std::pair<int, long long> _upperBound(Node* node, long long value);
  std::pair<int, long long> _antiUpperBound(Node* node, long long value);

  void _rightRotate(Node* node);
  void _leftRotate(Node* node);
  void _rotate(Node* node);

  void _zig(Node* node);
  void _zigZig(Node* node);
  void _zigZag(Node* node);

  void _splay(Node* node);

  void _searchKeyFromNode(Node* node, int key, int add);

  void _split(Node* t, Node*& l, Node*& r, int key);
  Node* _merge (Node* l, Node* r);
  Node* _merge (Node* left, Node* mid, Node* right);

  void _replaceChild (Node* parent, Node* old_child, Node* new_child);
  void _makeParent(Node* node, Node* new_parent);
  Node* _topParent(Node* node) const;

  void _assignNode (Node* node, long long value);
  void _addNode (Node* node, long long delta);
  void _reverseNode (Node* node);

  void _destroy(Node* node);

  void _recalc(Node* node);

  void _print(Node* node, bool detailed = false);

  static const long long INF = static_cast<long long>(2e18);
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


int SplayTree::size () const {
  return _root ? _root->size : 0;
}


void SplayTree::_replaceChild (Node* parent, Node* old_child, Node* new_child) {
  if (parent->left == old_child) {
    parent->left = new_child;
  } else if (parent->right == old_child) {
    parent->right = new_child;
  } else {
    throw std::invalid_argument("invalid old_child value in _replaceChild");
  }
  _recalc(parent);
}


void SplayTree::_rightRotate (SplayTree::Node* node) {

  node->parent->left = node->right;
  if (node->right != 0) {
    node->right->parent = node->parent;
  }
  node->right = node->parent;

  Node* grandParent = node->parent->parent;
  if (grandParent != 0) {
    _replaceChild(grandParent, node->parent, node);
  }
  node->parent->parent = node;
  node->parent = grandParent;

  _recalc(node->right);
  _recalc(node);
  _recalc(grandParent);

  _updateDP(node->right);
  _updateDP(node);
  _updateDP(grandParent);

}


void SplayTree::_leftRotate (SplayTree::Node* node) {

  node->parent->right = node->left;
  if (node->left != 0) {
    node->left->parent = node->parent;
  }
  node->left = node->parent;

  Node* grandParent = node->parent->parent;
  if (grandParent != 0) {
    _replaceChild(grandParent, node->parent, node);
  }
  node->parent->parent = node;
  node->parent = grandParent;

  _recalc(node->left);
  _recalc(node);
  _recalc(grandParent);

  _updateDP(node->left);
  _updateDP(node);
  _updateDP(grandParent);

}


void SplayTree::_rotate (SplayTree::Node* node) {
  if (node == node->parent->left) {
    _rightRotate(node);
  } else if (node == node->parent->right) {
    _leftRotate(node);
  } else {
    throw std::runtime_error("impossible scenario 1");
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
  if (node == 0) {
    return;
  } else if (node->parent == 0) {
    return;
  } else if (node->parent->parent == 0) {
    _zig(node);
  } else if (node->sameDirection() == true) {
    _zigZig(node);
  } else {
    _zigZag(node);
  }
  _splay(node);
}


int SplayTree::_getSize(SplayTree::Node* node) {
  return node ? node->size : 0;
}


void SplayTree::_searchKeyFromNode(SplayTree::Node* node, int key, int add) {

  _push(node);

  if (node == 0) {
    return;
  } else if (_getSize(node->left) + add + 1 == key) {
    _splay(node);
  } else if (_getSize(node->left) + add + 1 > key) {
    _searchKeyFromNode(node->left, key, add);
  } else if (_getSize(node->left) + add + 1 < key) {
    _searchKeyFromNode(node->right, key, add + 1 + _getSize(node->left));
  }
}


void SplayTree::_makeParent (SplayTree::Node* node, SplayTree::Node* new_parent) {
  if (node != 0) {
    node->parent = new_parent;
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


void SplayTree::_split (SplayTree::Node* t, SplayTree::Node*& l, SplayTree::Node*& r, int key) {
  if (key == 0 || t == 0) {
    l = 0;
    r = t;
    return;
  }

  _searchKeyFromNode(t, key, 0);
  t = _topParent(t);

  r = t->right;
  _makeParent(r, 0);

  l = t;
  l->right = 0;
  _recalc(t);
  _updateDP(t);
}


SplayTree::Node* SplayTree::_merge (SplayTree::Node* left, SplayTree::Node* right) {

  if (left == 0 || right == 0) {
    return left ? left : right;
  }

  if (left->parent != 0 || right->parent != 0) {
    throw std::runtime_error("nodes to merge are expected to not have parents");
  }

  _searchKeyFromNode(left, left->size, 0);

  left = _topParent(left);
  left->right = right;
  right->parent = left;

  _recalc(left);
  _updateDP(left);

  return left;
}


SplayTree::Node* SplayTree::_merge(SplayTree::Node* left, SplayTree::Node* mid, SplayTree::Node* right) {
  return _merge(_merge(left, mid), right);
}


void SplayTree::insert (long long value, int position) {
  if (_root == 0) {
    _root = new Node(value);
  } else {
    Node* left = 0;
    Node* right = 0;
    _split(_root, left, right, position);
    _root = new Node (value);
    _root = _merge(left, _root, right);
  }
}


bool SplayTree::erase (int key) {
  Node* left;
  Node* mid;
  Node* right;
  _split (_root, left, right, key);
  _split (left, left, mid, key-1);
  delete mid;
  _root = _merge(left, right);
  return true;
}


long long SplayTree::min () {
  if (_root == 0) {
    throw std::runtime_error("min can not be found in empty tree");
  }
  Node* current_vertex = _root;
  _push(current_vertex);
  while (current_vertex->left != 0) {
    _push(current_vertex);
    current_vertex = current_vertex->left;
  }
  _splay(current_vertex);
  _root = _topParent(current_vertex);
  return _root->value;
}


void SplayTree::_recalc (SplayTree::Node* node) {
  if (node != 0) {
    node->size = _getSize(node->left) + _getSize(node->right) + 1;
  }
}


void SplayTree::_print(SplayTree::Node* node, bool detailed) {
  if (node == 0) {
    return;
  }
  _push(node);
  if (node->left != 0) {
    if (detailed) {
      std::cout << "going left down" << std::endl;
    }
    _print(node->left, detailed);
    if (detailed) {
      std::cout << "going right up" << std::endl;
    }
  }
  std::cout << node->value << ' ';
  if (detailed) {
    std::cout << node->size << std::endl;
  }
  if (node->right != 0) {
    if (detailed) {
      std::cout << "going right down" << std::endl;
    }
    _print(node->right, detailed);
    if (detailed) {
      std::cout << "going left up" << std::endl;
    }
  }
}


void SplayTree::print(bool detailed) {
  if (detailed) {
    std::cout << "starting printing tree:" << std::endl;
  }
  if (_root != 0 && _root->parent != 0) {
    throw std::runtime_error("invalid _root value in print");
  }
  _print(_root, detailed);
  if (detailed) {
    std::cout << "finished printing tree." << std::endl;
  }
}


void SplayTree::_assignNode(Node* node, long long value) {
  if (node != 0) {
    node->is_added = false;
    node->added_value = 0;
    node->is_assigned = true;
    node->assigned_value = value;
  }
}


void SplayTree::_addNode(Node* node, long long delta) {
  if (node != 0) {
    if (node->is_assigned) {
      node->assigned_value += delta;
    } else if (node->is_added) {
      node->added_value += delta;
    } else {
      node->is_added = true;
      node->added_value = delta;
    }
  }
}


void SplayTree::_reverseNode(Node* node) {
  if (node != 0) {
    node->reversed ^= true;
  }
}


void SplayTree::_push(Node* node) {
  if (node == 0) {
    return;
  }
  if (node->is_assigned && node->is_added) {
    int N = 0; while(true) { ++N; } node->sum = N;
  }
  if (node->is_assigned) {
    node->is_assigned = false;
    node->value = node->assigned_value;
    node->leftest = node->assigned_value;
    node->rightest = node->assigned_value;
    node->sum = node->assigned_value * node->size;

    node->increasing_prefix = node->decreasing_prefix = node->increasing_suffix = node->decreasing_suffix = node->size;

    _assignNode(node->left, node->assigned_value);
    _assignNode(node->right, node->assigned_value);
    node->assigned_value = 0;
  }
  if (node->is_added) {
    node->is_added = false;
    node->sum += node->added_value * node->size;
    node->value += node->added_value;
    node->leftest += node->added_value;
    node->rightest += node->added_value;

    // prefixes and suffixes do not change

    _addNode(node->left, node->added_value);
    _addNode(node->right, node->added_value);
    node->added_value = 0;
  }
  if (node->reversed) {
    node->reversed = false;
    std::swap(node->left, node->right);
    std::swap(node->leftest, node->rightest);
    std::swap(node->increasing_prefix, node->decreasing_suffix);
    std::swap(node->decreasing_prefix, node->increasing_suffix);
    _reverseNode(node->left);
    _reverseNode(node->right);
  }
}


void SplayTree::makeRotation(int l, int r) {
  if (l == r) {
    return;
  }
  Node* left;
  Node* mid;
  Node* right;

  _split(_root, _root, right, r);
  _split(_root, left, mid, l-1);

  mid->reversed ^= true;

  _root = _merge(left, mid, right);
}


void SplayTree::assign (int l, int r, long long value) {
  Node* left;
  Node* mid;
  Node* right;

  _split(_root, mid, right, r);
  _split(mid, left, mid, l-1);

  mid->is_added = false;
  mid->added_value = 0;
  mid->is_assigned = true;
  mid->assigned_value = value;

  _root = _merge(left, mid, right);
}


void SplayTree::add (int l, int r, long long delta) {
  Node* left;
  Node* mid;
  Node* right;

  _split(_root, mid, right, r);
  _split(mid, left, mid, l-1);

  if (mid->is_assigned) {
    mid->assigned_value += delta;
  } else if (mid->is_added) {
    mid->added_value += delta;
  } else {
    mid->is_added = true;
    mid->added_value = delta;
  }

  _root = _merge(left, mid, right);
}


long long SplayTree::_getValue (SplayTree::Node* node) {
  return node == 0 ? 0 : node->value;
}

int SplayTree::_getIncreasingPrefix (SplayTree::Node* node) {
  return node == 0 ? 0 : node->increasing_prefix;
}

int SplayTree::_getIncreasingSuffix (SplayTree::Node* node) {
  return node == 0 ? 0 : node->increasing_suffix;
}

int SplayTree::_getDecreasingPrefix (SplayTree::Node* node) {
  return node == 0 ? 0 : node->decreasing_prefix;
}

int SplayTree::_getDecreasingSuffix (SplayTree::Node* node) {
  return node == 0 ? 0 : node->decreasing_suffix;
}

long long SplayTree::_getSum (SplayTree::Node* node) {
  return node == 0 ? 0 : node->sum;
}


void SplayTree::_updateDP (SplayTree::Node* node) {
  if (node == 0) {
    return;
  }

  _push(node);
  _push(node->left);
  _push(node->right);

  node->increasing_prefix = node->decreasing_prefix = node->increasing_suffix = node->decreasing_suffix = 0;

  node->sum = _getSum (node->left) + _getSum(node->right) + node->value;
  node->leftest = (node->left == 0 ? node->value : node->left->leftest);
  node->rightest = (node->right == 0 ? node->value : node->right->rightest);
  // irremovable copy-paste:
  // updating increasing prefix
  node->increasing_prefix = _getIncreasingPrefix(node->left);
  if ((node->left != 0 && node->left->increasing_prefix == node->left->size &&
      node->value >= node->left->rightest) || node->left == 0) {
    ++node->increasing_prefix;
    if (node->right != 0 && node->right->leftest >= node->value) {
      node->increasing_prefix += node->right->increasing_prefix;
	}
  }
  // updating decreasing prefix
  node->decreasing_prefix = _getDecreasingPrefix(node->left);
  if ((node->left != 0 && node->left->decreasing_prefix == node->left->size &&
      node->value <= node->left->rightest) || node->left == 0) {
    ++node->decreasing_prefix;
    if (node->right != 0 && node->right->leftest <= node->value) {
      node->decreasing_prefix += node->right->decreasing_prefix;
    }
  }
  // updating increasing suffix
  node->increasing_suffix = _getIncreasingSuffix(node->right);
  if ((node->right != 0 && node->right->increasing_suffix == node->right->size &&
      node->value <= node->right->leftest) || node->right == 0) {
    ++node->increasing_suffix;
    if (node->left != 0 && node->left->rightest <= node->value) {
      node->increasing_suffix += node->left->increasing_suffix;
    }
  }
  // updating decreasing suffix
  node->decreasing_suffix = _getDecreasingSuffix(node->right);
  if ((node->right != 0 && node->right->decreasing_suffix == node->right->size &&
      node->value >= node->right->leftest) || node->right == 0) {
    ++node->decreasing_suffix;
    if (node->left != 0 && node->left->rightest >= node->value) {
      node->decreasing_suffix += node->left->decreasing_suffix;
    }
  }
}


long long SplayTree::sum (int l, int r) {
  Node* left;
  Node* mid;
  Node* right;

  _split(_root, _root, right, r);
  _split(_root, left, mid, l-1);

  long long answer = mid->sum;
  _root = _merge(left, mid, right);
  return answer;
}


std::pair<int, long long> SplayTree::_upperBound(Node* node, long long target) {
  if (node == 0) {
    return {-1, -1};
  }
  _push(node);
  _push(node->left);
  _push(node->right);
  if (node->value > target) {
    std::pair<int, long long> possible_index_value = _upperBound(node->right, target);
    return possible_index_value.first == -1 ? std::make_pair(_getSize(node->left) + 1, node->value) :
        std::make_pair(_getSize(node->left) + 1 + possible_index_value.first, possible_index_value.second);
  } else {
    return _upperBound(node->left, target);
  }
}


void SplayTree::next_permutation (int l, int r) {
  Node* left;
  Node* mid;
  Node* right;

  _split(_root, _root, right, r);
  _split(_root, left, mid, l-1);

  int left_index = r - mid->decreasing_suffix;
  if (left_index < l) {
    _root = _merge(left, mid, right);
    makeRotation(l, r);
    return;
  }

  Node* midleft;
  _split(mid, midleft, mid, left_index - _getSize(left));

  long long target = midleft->rightest;
  std::pair<int, long long> index_value = _upperBound(mid, target);
  int delta_index = index_value.first;
  long long new_left_value = index_value.second;
  int right_index = left_index + delta_index;

  _root = _merge(_merge(left, midleft, mid), right);

  erase(left_index);
  insert(new_left_value, left_index-1);
  erase(right_index);
  insert(target, right_index-1);
  makeRotation(left_index + 1, r);

}


std::pair<int, long long> SplayTree::_antiUpperBound(Node* node, long long target) {
  if (node == 0) {
    return {-1, -1};
  }
  _push(node);
  _push(node->left);
  _push(node->right);
  if (node->value < target) {
    std::pair<int, long long> possible_index_value = _antiUpperBound(node->right, target);
    return possible_index_value.first == -1 ? std::make_pair(_getSize(node->left) + 1, node->value) :
        std::make_pair(_getSize(node->left) + 1 + possible_index_value.first, possible_index_value.second);
  } else {
    return _antiUpperBound(node->left, target);
  }
}


void SplayTree::prev_permutation (int l, int r) {
  Node* left;
  Node* mid;
  Node* right;

  _split(_root, _root, right, r);
  _split(_root, left, mid, l-1);

  int left_index = r - mid->increasing_suffix;
  if (left_index < l) {
    _root = _merge(left, mid, right);
    makeRotation(l, r);
    return;
  }

  Node* midleft;
  _split(mid, midleft, mid, left_index - _getSize(left));

  long long target = midleft->rightest;
  std::pair<int, long long> index_value = _antiUpperBound(mid, target);
  int delta_index = index_value.first;
  long long new_left_value = index_value.second;
  int right_index = left_index + delta_index;

  _root = _merge(_merge(left, midleft, mid), right);

  erase(left_index);
  insert(new_left_value, left_index-1);
  erase(right_index);
  insert(target, right_index-1);
  makeRotation(left_index + 1, r);

}


template<typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& v) {
  for (unsigned i = 0; i < v.size(); ++i) {
    os << v[i] << ' ';
  }
  return os;
}


void SplayTree::processRequests () {
  int type;
  std::cin >> type;
  if (type == 1) {
  int l, r;
  std::cin >> l >> r;
  std::cout << sum(l + 1, r + 1) << '\n';
  } else if (type == 2) {
    long long x;
    int pos;
    std::cin >> x >> pos;
    insert(x, pos);
  } else if (type == 3) {
    int pos;
    std::cin >> pos;
    erase(pos + 1);
  } else if (type == 4) {
    long long x;
    int l, r;
    std::cin >> x >> l >> r;
    assign(l + 1, r + 1, x);
  } else if (type == 5) {
    long long x;
    int l, r;
    std::cin >> x >> l >> r;
    add(l + 1, r + 1, x);
  } else if (type == 6) {
    int l, r;
    std::cin >> l >> r;
    next_permutation(l + 1, r + 1);
  } else if (type == 7) {
    int l, r;
    std::cin >> l >> r;
    prev_permutation(l + 1, r + 1);
  } else {
    throw std::runtime_error("unknown command");
  }
}
