#pragma once

#include "utility/mathutil.h"
#include "physbody.h"


// Quad tree node. Used for spatial indexing.
struct QuadNode {
	// Top-left position of the node.
	Vec2 pos;
	// Size of the node.
	Vec2 size;
	// Capacity (if exceeded, the node splits).
	unsigned capacity;
	// Pointers to all objects that are in this node (an object can appear in multiple nodes).
	std::vector<PhysBody*> object;
	// Children. If no children, all pointers are guaranteed to be NULL.
	QuadNode* child[4];

	QuadNode();
	QuadNode(Vec2 position, Vec2 size, unsigned max_elements);

	// Rebuild the tree from this node. This should be called on the root.
	// @param objects All PhysBody objects to send to the fitting nodes.
	void build(std::vector<PhysBody*>* objects);
	// Clear entire tree from this node. 
	void clear();
	// Fetch all leaves of the subtree from this node.
	// @return std::vector<QuadNode*> Pointers to leaf nodes.
	std::vector<QuadNode*> get_leaves();
	// @brief Gets all the nodes, good for iterating.
	// @return std::vector<QuadNode*> 
	std::vector<QuadNode*> get_all_nodes();
private:
	// @brief Insert an object to this node, and split the node if needed.
	// @param obj The PhysBody to add.
	void add(PhysBody* obj);
	// Create children.
	void split();
	// Pass objects to children. Should be called after split();
	void pass_objects();
};
