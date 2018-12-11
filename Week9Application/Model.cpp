#include "Week9Application.h"

#define _USE_MATH_DEFINES 1
#include <math.h>

#include "StdAfx.h"


#include "Model.h"
#include <algorithm>
#include <vector>
#include <map>    
#include <fstream>
#include <sstream>

Model::Model(char * filename)
{
	vertexNormals_found = false;
	parseObjFile(filename);

	loaddedSuccefully = true;

	if (vertexNormals_found == false)
		calculateNormals();

	showWireframe = false;

	glGenBuffers(1, &vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexArrayHandle);
	glBindVertexArray(vertexArrayHandle);
	glEnableVertexAttribArray(0 /* index */);
	glVertexAttribPointer(0 /* index */, 3 /* components */, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(1 /* index */);
	glVertexAttribPointer(1 /* index */, 3 /* components */, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2 /* index */);
	glVertexAttribPointer(2 /* index */, 2 /* components */, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, st));

}


Model::~Model(void)
{
	delete vertices;
	delete indices;
}


void Model::render()
{
	glBindVertexArray(vertexArrayHandle);
	if (renderWireframe())
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_POINT);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glPolygonMode(GL_FRONT, GL_FILL);
	}
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, indices);
}

typedef struct Triple
{
	float x;
	float y;
	float z;
} Triple;

typedef struct Face
{
	int triangle[3];
	int texcoord[3];
	int normcoord[3];
} Face;


typedef struct VertTextureNormalIndex
{
	VertTextureNormalIndex(int cnt, int v_idx, int t_idx, int n_idx) : indexCnt(cnt), v(v_idx), t(t_idx), n(n_idx)
	{}
	int indexCnt;
	int v;
	int t;
	int n;
} VTN;

struct VTN_finder
{
	VTN_finder(int v_idx, int t_idx, int n_idx) : key_v(v_idx), key_t(t_idx), key_n(n_idx)
	{}

	bool operator()(const VTN& o) const
	{
		return (o.v == key_v && o.t == key_t && o.n == key_n);
	}

	const int key_v;
	const int key_t;
	const int key_n;
};



bool Model::parseObjFile(char * filename)
{
	std::vector <Triple *> vertexCoordinates;
	std::vector <Triple *> textureCoordinates;
	std::vector <Triple *> normalCoordinates;

	std::vector <Face *> faces;

	// The model files are found in '$(ProjectDir)\resources\models\'
	std::string modelfullpath = "resources\\models\\";
	modelfullpath.append(filename);

	std::ifstream fl(modelfullpath);
	if (fl.is_open())
	{
		std::string line;
		while (fl.good())
		{
			getline(fl, line);

			if (line.length() > 0)
			{
				switch (line[0])
				{
				case '#': // comment, ignore
					break;
				case 'g': // new group - ignore for the moment.
					break;
				case 'v': // vertex, or texture coordinate.
				{
					std::stringstream sline(line);
					char c;

					if (line[1] == 't')
					{
						Triple * p = new Triple();
						sline >> c >> c >> (*p).x >> (*p).y >> (*p).z;
						textureCoordinates.push_back(p);
					}
					else if (line[1] == 'n')  // Vertex normals
					{
						vertexNormals_found = true;
						Triple * norm = new Triple();
						sline >> c >> c >> (*norm).x >> (*norm).y >> (*norm).z;

						normalCoordinates.push_back(norm);
					}
					else
					{
						Triple * coord = new Triple();
						sline >> c >> (*coord).x >> (*coord).y >> (*coord).z;

						vertexCoordinates.push_back(coord);
					}
				}
				break;
				case 'f': // face description
				{
					std::stringstream sline(line);
					char c;
					sline >> c;  //consume 'f' character
					Face * f = new Face();
					int count = 0;
					while (true)
					{
						int vertex;
						if (!sline.good())
							break;
						sline >> vertex;
						if (sline.fail()) {
							sline.clear();
							break;
						}
						if (count >= 3)
						{
							faces.push_back(f);
							Face * nf = new Face();
							(*nf).triangle[0] = (*f).triangle[0];
							(*nf).triangle[1] = (*f).triangle[2];
							(*nf).texcoord[0] = (*f).texcoord[0];
							(*nf).texcoord[1] = (*f).texcoord[2];
							(*nf).normcoord[0] = (*f).normcoord[0];
							(*nf).normcoord[1] = (*f).normcoord[2];
							f = nf;
							count--;
						}
						(*f).triangle[count] = vertex - 1; // start from 0

														   // 3 options here:
														   //    f 1/1 2/2 3/3				<-- Original code handled this...
														   //    f 1//1 2//2 3//3			<-- What Blender prduces...
														   //    f 1/2/1 2/3/2 3/4/3		<-- **** Cant handle this yet!!!
						sline.get(c);
						if (c == '/') // have texture.
						{
							if (sline.peek() == '/') // need to strip an extra '/' from OBJ files exported from Blender...
							{
								sline.get(c);
								int normcoord;
								sline >> normcoord;
								(*f).normcoord[count] = normcoord - 1; // start from 0
								(*f).texcoord[count] = 0; // Dont have a texture bvalue... just set it to null
							}
							else
							{
								int texture;
								sline >> texture;
								(*f).texcoord[count] = texture - 1; // start from 0

								if (sline.peek() == '/') // we now have a normal to read in...
								{
									sline.get(c); // consume '/'
									int normcoord;
									sline >> normcoord;
									(*f).normcoord[count] = normcoord - 1; // start from 0
								}
							}
						}
						count++;
					}
					faces.push_back(f);
				}
				break;
				}
			}
		}
		fl.close();
	}
	else
	{
		// Error in openning file!

		// Free up memory...
		while (!vertexCoordinates.empty())
		{
			Triple *vp = vertexCoordinates.back();
			delete vp;
			faces.pop_back();
		}

		while (!textureCoordinates.empty())
		{
			Triple *tp = textureCoordinates.back();
			delete tp;
			faces.pop_back();
		}

		while (!normalCoordinates.empty())
		{
			Triple *np = normalCoordinates.back();
			delete np;
			faces.pop_back();
		}

		while (!faces.empty())
		{
			Face *fp = faces.back();
			delete fp;
			faces.pop_back();
		}

		return false;
	}


	// Likely to have more texture coordinates than vertices since some 
	// texture coordinates cannot be duplicated. Try to combine these into
	// unique set.

	// If the number of texture coordinates is smaller, then some are probably duplicated. Check for
	// faces that are orthogonal to the unwrapping.

	// Making assumption that all faces have texture and vertex indices if any have i.e. all or none.


	typedef struct _VVV {
		int v;
		int t;
		int n;
	} VVV;


	std::vector <VVV> vertexTempVector;

	// ** NEW WAY ***
	typedef std::map <std::tuple<int, int, int>, int> VertexMapType;
	VertexMapType myVertexMap;
	// **************

	numTriangles = faces.size(); // triangles 
	numIndices = 3 * numTriangles;
	indices = new int[numIndices];
	int indiceCnt = 0;

	int* newindices = new int[numIndices];
	int newindiceCnt = 0;
	int vCount = 0;

	for (size_t findex = 0; findex < faces.size(); findex++)
	{
		for (int tri = 0; tri < 3; tri++)
		{
			VVV v;

			v.v = faces[findex]->triangle[tri];
			v.t = faces[findex]->texcoord[tri];
			v.n = faces[findex]->normcoord[tri];


			// **** NEW WAY LOOKUP USING MAPS ***
			typedef std::tuple<int, int, int> TupleType;
			TupleType myTuple = std::make_tuple(v.v, v.t, v.n);

			VertexMapType::iterator it2 = myVertexMap.find(myTuple);
			if (it2 != myVertexMap.end())
			{
				indices[indiceCnt++] = (*it2).second;
			}
			else
			{
				vertexTempVector.push_back(v);
				indices[indiceCnt++] = vCount;
				myVertexMap[myTuple] = vCount;
				vCount++;
			}
			// **** NEW WAY LOOKUP USING MAPS ***
		}
	}


	// Copy values into internal structures.
	numVertices = vertexTempVector.size();
	vertices = new Vertex[numVertices];


	// Copy the vertex data into buffer first...
	for (std::vector<int>::size_type index = 0; index < vertexTempVector.size(); index++)
	{
		vertices[index].position[0] = vertexCoordinates[vertexTempVector[index].v]->x;
		vertices[index].position[1] = vertexCoordinates[vertexTempVector[index].v]->y;
		vertices[index].position[2] = vertexCoordinates[vertexTempVector[index].v]->z;

		if (textureCoordinates.size() > 0)
		{
			vertices[index].st[0] = textureCoordinates[vertexTempVector[index].t]->x;
			vertices[index].st[1] = textureCoordinates[vertexTempVector[index].t]->y;
		}
		else
		{
			vertices[index].st[0] = 0.0;
			vertices[index].st[1] = 0.0;
		}

		if (normalCoordinates.size() > 0)
		{
			vertices[index].normal[0] = normalCoordinates[vertexTempVector[index].n]->x;
			vertices[index].normal[1] = normalCoordinates[vertexTempVector[index].n]->y;
			vertices[index].normal[2] = normalCoordinates[vertexTempVector[index].n]->z;
		}
		else
		{
			vertices[index].normal[0] = 0.0;
			vertices[index].normal[1] = 0.0;
			vertices[index].normal[2] = 0.0;
		}
	}

	while (!vertexCoordinates.empty())
	{
		Triple *vp = vertexCoordinates.back();
		delete vp;
		vertexCoordinates.pop_back();
	}

	while (!textureCoordinates.empty())
	{
		Triple *tp = textureCoordinates.back();
		delete tp;
		textureCoordinates.pop_back();
	}

	while (!normalCoordinates.empty())
	{
		Triple *np = normalCoordinates.back();
		delete np;
		normalCoordinates.pop_back();
	}

	while (!faces.empty())
	{
		Face *fp = faces.back();
		delete fp;
		faces.pop_back();
	}

	return true;
}


void Model::calculateNormals()
{
	// zero each vertex normal.
	for (int i = 0; i < numVertices; i++)
	{
		vertices[i].normal[0] = 0.0f;
		vertices[i].normal[1] = 0.0f;
		vertices[i].normal[2] = 0.0f;
	}

	// accumulate each face normal.
	for (int i = 0; i < numTriangles; i++)
	{
		float * A = (float *) &(vertices[indices[3 * i + 0]].position);
		float * B = (float *) &(vertices[indices[3 * i + 1]].position);
		float * C = (float *) &(vertices[indices[3 * i + 2]].position);

		vec3 side1 = vec3(C[0], C[1], C[2]) - vec3(A[0], A[1], A[2]);
		vec3 side2 = vec3(B[0], B[1], B[2]) - vec3(A[0], A[1], A[2]);
		vec3 normal = glm::cross(side2, side1); // not normalizing, so bigger triangles count more.

		for (int v = 0; v < 3; v++)
		{
			vertices[indices[3 * i + v]].normal[0] += normal.x;
			vertices[indices[3 * i + v]].normal[1] += normal.y;
			vertices[indices[3 * i + v]].normal[2] += normal.z;
		}
	}

	// normalize all normals!
	for (int i = 0; i < numVertices; i++)
	{
		float * N = (float *) &(vertices[i].normal);

		vec3 normal = glm::normalize(vec3(N[0], N[1], N[2]));

		N[0] = normal.x;
		N[1] = normal.y;
		N[2] = normal.z;
	}
}
