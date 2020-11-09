#pragma once

template <typename T>
class SmArray {
private:
	T* mem;
	size_t Num;
	size_t max_size;
public:

	T& at(size_t i) {
		//if (i + 1 > current_size)
		//	assert(0);
		return mem[i];
	}

	T& operator[] (size_t index) {
		return at(index);
	}

	void resize(size_t news) {
		Num = news;
	}

	void clear() {
		Num = 0;
	}

	void push_back(T& item) {
		if (Num < max_size) {
			mem[Num] = item;
			Num++;
		}
	}

	void push_back_Unique(T& item) {
		if (!isInList(item))
			push_back(item);
	}

	void pop_back() {
		if (Num > 0)
			Num--;
	}

	bool isInList(T& item) {
		for (size_t i = 0; i < max_size; i++) { //Iterating over all objects
			if (mem[i] == item) { //If object marked as deleted
				return true;
			}
		}
		return false;
	}

	void _delete(T& item) {
		for (size_t i = 0; i < Num; i++) { //Iterating over all objects
			if (mem[i] == item) { //If object marked as deleted
				for (size_t obj_i = i + 1; obj_i <= Num; obj_i++) { //Iterate over all next chidren
					mem[obj_i - 1] = mem[obj_i]; //Move it to previous place
				}
				pop_back(); //remove last position
			}
		}
	}

	size_t size() {
		return Num;
	}

	void reserve(size_t size) {
		mem = new T[size];
		max_size = size;
	}
	SmArray() {
		Num = 0;
		mem = nullptr;
	}
	SmArray(size_t size) {
		reserve(size);
		Num = 0;
	}
	~SmArray() {
		if (mem != nullptr)
			delete[] mem;
	}
};