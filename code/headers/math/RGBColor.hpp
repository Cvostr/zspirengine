#pragma once

class ZSRGBCOLOR {
public:
	int r;
	int g;
	int b;
	int a;

	float gl_r;
	float gl_g;
	float gl_b;
	float gl_a;

	void updateGL();


	ZSRGBCOLOR& operator=(ZSRGBCOLOR r) {
		this->r = r.r;
		g = r.g;
		b = r.b;
		a = r.a;
		this->updateGL();
		return *this;
	}

	ZSRGBCOLOR(int r, int g, int b, int a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;

		this->updateGL();
	}
	ZSRGBCOLOR(int r, int g, int b) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;

		this->updateGL();
	}

	ZSRGBCOLOR() {
		this->r = 255;
		this->g = 255;
		this->b = 255;
		this->a = 255;

		this->updateGL();
	}

};