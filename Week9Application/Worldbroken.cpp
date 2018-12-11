#include "Week9Application.h"

#include "stdafx.h"
#include "Shader.h"
#include "Model.h"
#include "World.h"

WorldObject::WorldObject(Model * model, Shader * shader) : model(model), shader(shader)
{
	setPosition(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	up[0] = 0.0f;
	up[1] = 1.0f;
	up[2] = 0.0f;
}

void WorldObject::setPosition(float x, float y, float z, float forwardx, float forwardy, float forwardz)

{
	modelPosition[0] = x;
	modelPosition[1] = y;
	modelPosition[2] = z;

	forward[0] = forwardx;
	forward[1] = forwardy;
	forward[2] = forwardz;
}

mat4 WorldObject::getWorldTransform()
{
	vec3 dirforward(forward[0], forward[1], forward[2]);
	vec3 dirup(up[0], up[1], up[2]);
	vec3 dirside = glm::cross(dirup, dirforward);

	// in this format, goes in as the transpose.
	mat4 rot = mat4(dirside[0], dirup[0], dirforward[0], 0.0,
					dirside[1], dirup[1], dirforward[1], 0.0,
					dirside[2], dirup[2], dirforward[2], 0.0,
					0.0, 0.0, 0.0, 1.0);
	mat4 M = glm::translate(mat4(1.0f), vec3(modelPosition[0], modelPosition[1], modelPosition[2])) * rot * glm::scale(mat4(1.0f), vec3(1.0f / 3.0f));
	return M;
}


World::World(void)
{

	const int maxModels = 22; // from 21 to 22 to extend/add another object. 
	models = new WorldObject *[maxModels];
	numModels = 0;

	//  Model * king = new Model ((char *) "cube-flat.obj");

	Model * king = new Model((char *) "chess/kingNT2.obj");
	Model * queen = new Model((char *) "chess/queenNT.obj");
	Model * knight = new Model((char *) "chess/knightNT2.obj");
	Model * bishop = new Model((char *) "chess/bishopNT.obj");
	Model * rook = new Model((char *) "chess/rookNT.obj");
	Model * pawn = new Model((char *) "chess/pawnNT.obj");
	Model * ground = new Model((char *) "plane.obj");
	Model * board = new Model((char *) "board.obj");
	Model * car = new Model((char *) "car.obj");
	Model * tree = new Model((char *) "tree3.obj");
	Model * car2 = new Model((char *) "car.obj");		// New object to asset. 
	//Model * ttree = new Model((char *) "tree4.obj");

	marbleShader = new Shader((char *) "modelVS.glsl", (char *) "emptyGS.glsl", (char *) "marbleFS.glsl");
	ebonyShader = new Shader((char *) "modelVS.glsl", (char *) "emptyGS.glsl", (char *) "ebonyFS.glsl");
	boardShader = new Shader((char *) "boardVS.glsl", (char *) "boardGS.glsl", (char *) "boardFS.glsl");
	groundShader = new Shader((char *) "groundVS.glsl", (char *) "groundGS.glsl", (char *) "groundFS.glsl");
	treeShader = new Shader((char *) "modelVS.glsl", (char *) "emptyGS.glsl", (char *) "treeFS.glsl");
	carShader = new Shader((char *) "modelVS.glsl", (char *) "emptyGS.glsl", (char *) "carFS.glsl");
	car2Shader = new Shader((char *) "sinusoidalVS.glsl", (char *) "emptyGS.glsl", (char *) "carFS.glsl"); // *********************
	


	// Assign only one texture to the board shader 
	//(GROUND PLANE/OBJECT**)
	boardShader->assignTextureToSlot("bsnow.jpg", 0);

	// Assign three (max) textures to the ground shader for testing
	groundShader->assignTextureToSlot("testtexture.png", 0);
	groundShader->assignTextureToSlot("tex07.jpg", 1);
	groundShader->assignTextureToSlot("textureimage.jpg", 2);

	models[numModels] = new WorldObject(knight, marbleShader);
	models[numModels]->setPosition(0.0f, 1.0f, 0.0f, 0.0, 0.0, -1.0f);
	numModels++;

	for (int i = 1; i < 5; i++)
	{
		float stepSize = 0.8f;
		Shader *shaderToUse;
		if (i % 2 == 0)	// Choose every second step
		{
			shaderToUse = marbleShader;
		}
		else
		{
			shaderToUse = ebonyShader;
		}
		models[numModels] = new WorldObject(pawn, shaderToUse);
		models[numModels]->setPosition(i * stepSize, 0.0, i * stepSize, 0.0, 0.0, -1.0f);
		numModels++;

		models[numModels] = new WorldObject(pawn, shaderToUse);
		models[numModels]->setPosition(i * -stepSize, 0.0, i * -stepSize, 0.0, 0.0, -1.0f);
		numModels++;

		models[numModels] = new WorldObject(pawn, shaderToUse);
		models[numModels]->setPosition(i * -stepSize, 0.0, i * stepSize, 0.0, 0.0, -1.0f);
		numModels++;

		models[numModels] = new WorldObject(pawn, shaderToUse);
		models[numModels]->setPosition(i * stepSize, 0.0, i * -stepSize, 0.0, 0.0, -1.0f);
		numModels++;
	}


	models[numModels] = new WorldObject(ground, groundShader);
	models[numModels]->setPosition(0.0f, 0.0f, 0.0f, 0.0, 0.0, 1.0f);
	numModels++;

	models[numModels] = new WorldObject(board, boardShader);
	models[numModels]->setPosition(0.0f, 0.0f, 0.0f, 0.0, 0.0, 1.0f);
	numModels++;

	models[numModels] = new WorldObject(car, carShader);
	models[numModels]->setPosition(5.0f, 0.0f, 0.0f, 0.0, 0.0, 1.0f);
	numModels++;

	models[numModels] = new WorldObject(tree, treeShader);
	models[numModels]->setPosition(-5.0f, 0.0f, 0.0f, 0.0, 0.0, 1.0f);
	numModels++;

	models[numModels] = new WorldObject(car2, car2Shader);
	models[numModels]->setPosition(0.0f, 0.0f, 0.0f, 0.0, 0.0, 1.0f); // *********************
	numModels++;
	assert(numModels == maxModels);

	// Initialise the light position
	lx = 0.5;
	ly = 30.5;
	lz = 2.0;
}


World::~World(void)
{

}

//static float pauto = 0.0f; // ** GLOBAL VARIABLE DEFINED ** 

void World::renderSpecific(WorldObject * object, mat4 view, mat4 proj, mat4 shadow)
{
	static float pauto;

	Model * model = object->getModel();
	mat4 world = object->getWorldTransform();
	Shader * shader = object->getShader();

	shader->activateShader();

	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), (char *) "WorldMatrix"), 1, GL_FALSE, &world[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), (char *) "ViewMatrix"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), (char *) "ProjectionMatrix"), 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->getProgram(), (char *) "ShadowMatrix"), 1, GL_FALSE, &shadow[0][0]);

	glUniform1i(glGetUniformLocation(shader->getProgram(), (char *) "ShadowMap"), 0);
	glUniform4f(glGetUniformLocation(shader->getProgram(), (char *) "LightPosition"), lx, ly, lz, 1.0f);

	vec2 windowSize = getWindowSize();
	glUniform2f(glGetUniformLocation(shader->getProgram(), (char *) "u_resolution"), windowSize.x, windowSize.y);
	
	glUniform2f(glGetUniformLocation(shader->getProgram(), (char *) "pauto"), windowSize.x, windowSize.y);
	pauto += 0.1f; //Increasing counter to pauto  

	glUniform1i(glGetUniformLocation(shader->getProgram(), "texture1"), shader->getTexture1());
	glUniform1i(glGetUniformLocation(shader->getProgram(), "texture2"), shader->getTexture2());
	glUniform1i(glGetUniformLocation(shader->getProgram(), "texture3"), shader->getTexture3());

	model->render();
}

void World::render(mat4 view, mat4 proj, mat4 shadow)
{

	for (int i = 0; i < numModels; i++)
	{
		renderSpecific(models[i], view, proj, shadow);
	}
}
