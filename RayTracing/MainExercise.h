#pragma once

enum  ProgramMode { /*GETTING_POINTS = 0x1, DRAW_POLYGON = 0x2, COLORING = 0x4*/ };

class MainExercise
{
public:
	void run(int argc, char* argv[]);
	void init(int windowWidth, int windowHeight);

	static void display();
	static void reshape(int width, int height);
	static void OnMouseClick(int button, int state, int x, int y);
	static void OnKeyboardClick(unsigned char key, int x, int y);

	int getWindHeight() { return _windowHeight; }
	int getWindWidth() { return _windowWidth; }
private:
	int _windowWidth = 400;
	int _windowHeight = 400;

};

