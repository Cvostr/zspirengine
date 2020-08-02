#pragma once

class RefCounter {
public:
	int refsC;

	void AddRef() {
		refsC++;
	}
	void Release() {
		refsC--;
	}

	RefCounter() {
		refsC = 1;
	}
};