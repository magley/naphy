#pragma once

#include "utility/mathutil.h"
#include "physbody.h"



struct QuadNode {
	/**
	 * @brief Top-left position of the node.
	 */
	Vec2 pos;
	/**
	 * @brief Size of the node.
	 */
	Vec2 size;
	/**
	 * @brief Capacity (if exceeded, the node splits).
	 */
	unsigned capacity;
	/**
	 * @brief All objects that are in this node (multiple nodes can "own" an object).
	 */
	std::vector<PhysBody*> object;
	/**
	 * @brief Children. If no children, all must be NULL.
	 */
	QuadNode* child[4];

	QuadNode();
	QuadNode(Vec2 position, Vec2 size, unsigned max_elements);

	/**
	 * @brief Rebuild, should be called on the root.
	 * @param objects All objects to pass to the tree.
	 */
	void build(std::vector<PhysBody*>* objects);
	/**
	 * @brief Clear everything.
	 */
	void clear();
	/**
	 * @brief Fetch all leaves of the subtree from this node.
	 * @return std::vector<QuadNode*> Pointers to leaf nodes.
	 */
	std::vector<QuadNode*> get_leaves();
	/**
	 * @brief Gets all the nodes, good for iterating.
	 * 
	 * @return std::vector<QuadNode*> 
	 */
	std::vector<QuadNode*> get_all_nodes();
private:
	/**
	 * @brief Insert an object to this node, split if needed.
	 * @param obj The physbody to add.
	 */
	void add(PhysBody* obj);
	/**
	 * @brief Create children.
	 */
	void split();
	/**
	 * @brief Pass objects to children. Should be called after split();
	 */
	void pass_objects();
};
