#include <engine/node.hpp>

Engine::Node::Node(std::string nname) : name(nname) {}
Engine::Node::Node() : name("Node") {}

void Engine::Node::add_child(Node* child) {
	childs.push_back(child);
}
void Engine::Node::print_tree_from_here(int depth = 0) {
	for (int i = 0; i < depth; i++) std::cout << "  ";
	std::cout << name << "\n";

	for (auto ch : childs) {
		ch->print_tree_from_here(depth+1);
	}
}
void Engine::Node::apply_to_tree(void(func)(Node*)) {
	func(this);
	for (auto ch : childs) {
		ch->apply_to_tree(func);
	}
}
