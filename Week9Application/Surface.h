#include "Week9Application.h"

#include "World.h"
#include "Model.h"

#pragma once

// The visualization element. 
class Surface
{
private:
	HWND handle;
	HDC hDC;
	void init();

	int		windowWidth;
	int		windowHeight;

protected:
	// Link to the model; in this case the set
	// of models in the world that we're creating.
	World * world;

	float cameraPosition[3];
	float cameraForward[3];
	float cameraUp[3];
	float speed;
	float angleSpeed;


	mat3 rotationVectorAngle(float ux, float uy, float uz, float angle);

public:
	Surface(HINSTANCE hInst, HWND hWnd);
	~Surface(void);
	// resize surface as well as reset the graphics viewport.
	void resize(int width, int height);

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	// move sideways.
	void strafe(float direction);
	void step(float direction);
	void turnside(float direction);
	void turnup(float direction);
};


