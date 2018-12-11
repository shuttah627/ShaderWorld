#pragma once

#include "Week9Application.h"

class Model;
class Shader;

class WorldObject
{
private:
	Model * model;
	Shader * shader;
	float modelPosition[3];
	float forward[3];
	float up[3];

public:
	WorldObject(Model * model, Shader * shader);
	~WorldObject();
	void setPosition(float x, float y, float z, float forwardx, float forwardy, float forwardz);
	mat4 getWorldTransform();

	Model * getModel() { return model; }
	Shader * getShader() { return shader; }
};

class World
{
private:
	WorldObject * * models;
	int numModels;

	Shader * treeShader;
	Shader * carShader;
	Shader * car2Shader;

	float lx;
	float ly;
	float lz;
	
	vec2	windowSize;

	void renderSpecific(WorldObject * object, mat4 view, mat4 proj, mat4 shadow);

public:
	World(void);
	~World(void);

	void setWindowSize(int width, int height) { windowSize.x = float(width); windowSize.y = float(height); return; };
	vec2 getWindowSize() { return windowSize; };

	void render (mat4 view, mat4 proj, mat4 shadow);
};
