#pragma once

#include <vector>
#include <functional>

namespace Engine {

	template<typename... args>
	class Signal {
	private:
		std::vector<std::function<void(args...)>> connected;

	public:
		void connect(void(function)(args...));
		void disconnect(void(function)(args...));
		void emit(args... a);
	};

	#include "../../src/engine/signal.tpp"

};
