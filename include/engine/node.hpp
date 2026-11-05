#pragma once

namespace Engine {

	class Node {
	public:
		std::string name;

		Node(std::string nnmae);
		Node();

		Node* parent;
		std::vector<Node*> childs;

		void apply_to_tree(void(func)(Node*));

		void add_child(Node* child);
		void print_tree_from_here(int depth);
	};

};
