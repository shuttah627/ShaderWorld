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
	srand(time(NULL));
	const int maxModels = 3;
	models = new WorldObject *[maxModels];
	numModels = 0;

	Model * car = new Model((char *) "plane-hires.obj");
	Model * tree = new Model((char *) "boat.obj");

	treeShader = new Shader((char *) "floatVS.glsl", (char *) "emptyGS.glsl", (char *) "treeFS.glsl");
	carShader = new Shader((char *) "planeVS.glsl", (char *) "emptyGS.glsl", (char *) "planeFS.glsl");

	models[numModels] = new WorldObject(car, carShader);
	models[numModels]->setPosition(0.0f, 0.0f, 0.0f, 0.0, 0.0, 1.0f);
	numModels++;

	models[numModels] = new WorldObject(tree, treeShader);
	models[numModels]->setPosition(0.0f, -4.0f, 0.0f, 0.0, 0.0, 1.0f);
	numModels++;

	assert(numModels == maxModels);

	// Initialise the light position
	lx = 0.0;
	ly = -3.5;
	lz = 0.0;
}


World::~World(void)
{

}

void World::renderSpecific(WorldObject * object, mat4 view, mat4 proj, mat4 shadow)
{
		static float pauto = 0.0f;        // Define a global variable that is only available to this method.
		float ranNum = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

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

		// Pass in an incrementing counter to the shader
		glUniform1f(glGetUniformLocation(shader->getProgram(), (char *) "pauto"), pauto);
		pauto += 0.05f;

		glUniform1f(glGetUniformLocation(shader->getProgram(), (char *) "ranNum"), ranNum);

		glUniform1i(glGetUniformLocation(shader->getProgram(), "texture1"), shader->getTexture1());
		glUniform1i(glGetUniformLocation(shader->getProgram(), "texture2"), shader->getTexture2());
		glUniform1i(glGetUniformLocation(shader->getProgram(), "texture3"), shader->getTexture3());
		//ly += sin((pauto / 1000)) * 5;
		model->render();
}

void World::render(mat4 view, mat4 proj, mat4 shadow)
{

	for (int i = 0; i < numModels; i++)
	{
		renderSpecific(models[i], view, proj, shadow);
	}
}
