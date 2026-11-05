template<typename... args>
void Signal<args...>::connect(void(*function)(args...)) {
	connected.push_back(function);
}

template<typename... args>
void Signal<args...>::disconnect(void(*function)(args...)) {
	for(int i = 0; i < connected.size(); i++) {
    	auto tmp_func = connected[i]. template target<void(*)(args...)>();

		if (tmp_func && (function == *tmp_func)) {
			connected.erase(connected.begin() + i); 
			break;
		}
	}
}
template<typename... args>
void Signal<args...>::emit(args... a) {
	for (auto func : connected) {
		func(a...);
	}
}
