#pragma once

#include "Week9Application.h"

typedef struct Vertex
{
	float position[3];
	float normal[3];
	float st[2];
} Vertex;

class Model
{
public:
	Model(char * filename);
	~Model(void);

protected:
	int		numVertices;
	struct Vertex * vertices;    // coordinates of the vertices of all the polygons.
	int		numTriangles;
	int		numIndices;
	int	*	indices;       // indices of vertices in the polygons.
	bool	vertexNormals_found;
	bool	loaddedSuccefully = false;
	bool	showWireframe = false;

	GLuint vertexBufferHandle;
	GLuint vertexArrayHandle;

	bool	parseObjFile(char * filename);
	void	calculateNormals();

public:
	bool	loadOK() { return loaddedSuccefully; }	// inline method
	void	setWireframe(bool status) { showWireframe = status; }
	bool	renderWireframe() { return showWireframe; }

	void render();

};
