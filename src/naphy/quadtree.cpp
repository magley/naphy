#include "quadtree.h"
#include "assert.h"
#include <queue>


QuadNode::QuadNode() {
	capacity = 0;
	child[0] = child[1] = child[2] = child[3] = NULL;
}

QuadNode::QuadNode(Vec2 position, Vec2 size, unsigned max_elements) {
	pos = position;
	this->size = size;
	capacity = max_elements;
	child[0] = child[1] = child[2] = child[3] = NULL;
}


void QuadNode::build(std::vector<PhysBody>* objects) {
	// Only for building the whole tree from zero.
	// It adds everything into the root first and
	// only then does it check for overflow. It's
	// faster than checking for overflow after each
	// insert.

	for (unsigned i = 0; i < objects->size(); i++) {
		PhysBody* obj = &(*objects)[i];

		if (obj->bbox_query(pos, pos + size))
			object.push_back(obj);
	}

	if (object.size() > capacity) {
		split();
		pass_objects();
	}
}

void QuadNode::clear() {
	object.clear();

	if (child[0] == NULL)
		return;

	for (unsigned i = 0; i < 4; i++) {
		child[i]->clear();
		delete child[i];
		child[i] = NULL;
	}
}

std::vector<QuadNode*> QuadNode::get_leaves() {
	std::vector<QuadNode*> v;
	std::queue<QuadNode*> q;

	q.push(this);

	while (!q.empty()) {
		QuadNode* node = q.front();
		q.pop();

		if (node->child[0] == NULL) {
			v.push_back(node);
		} else {
			q.push(node->child[0]);
			q.push(node->child[1]);
			q.push(node->child[2]);
			q.push(node->child[3]);
		}
	}

	return v;
}

std::vector<QuadNode*> QuadNode::get_all_nodes() {
	std::vector<QuadNode*> v;
	std::queue<QuadNode*> q;

	q.push(this);

	while (!q.empty()) {
		QuadNode* node = q.front();
		q.pop();

		v.push_back(node);
		if (node->child[0] != NULL) {
			q.push(node->child[0]);
			q.push(node->child[1]);
			q.push(node->child[2]);
			q.push(node->child[3]);
		}
	}

	return v;
}

void QuadNode::add(PhysBody* obj) {
	if (child[0] == NULL) {
		object.push_back(obj);

		if (object.size() > capacity) {
			if (size.x <= 64)
				return;
			split();
			pass_objects();
		}
	} else {
		for (unsigned i = 0; i < 4; i++) {
			if (obj->bbox_query(child[i]->pos, child[i]->pos + child[i]->size)) {
				child[i]->add(obj);
			}
		}
	}
}

void QuadNode::split() {
	assert(child[0] == NULL);

	const double w = size.x / 2;
	const double h = size.y / 2;
	child[0] = new QuadNode(pos + Vec2(0, 0), size / 2, capacity);
	child[1] = new QuadNode(pos + Vec2(w, 0), size / 2, capacity);
	child[2] = new QuadNode(pos + Vec2(0, h), size / 2, capacity);
	child[3] = new QuadNode(pos + Vec2(w, h), size / 2, capacity);
}

void QuadNode::pass_objects() {
	assert(child[0] != NULL);

	for (unsigned i = 0; i < object.size(); i++) {
		for (unsigned j = 0; j < 4; j++) {
			if (object[i]->bbox_query(child[j]->pos, child[j]->pos + child[j]->size))
				child[j]->add(object[i]);
		}
	}

	object.clear();
}