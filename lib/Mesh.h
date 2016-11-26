#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

struct Vertex 		//minimal requirements for a mesh. A mesh needs a set of verticies where each pisition contains a vector,normal vector, and tex coord vector.
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords; //tex coordinates can only have x,y coord, thats why vec2
}

struct Texture // storing the id of the texture and the type of texture.
{
	GLuint id;
	string type;
}

class Mesh
{
private:
	GLuint VAO, VBO, EBO; // rendering data
	void setMesh();		//function to set the mesh
public:
	// mesh data //
	std::vector<Vertex> verticies;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	// functions //
	Mesh(std::vector<Vertex> verticies, std::vector<GLuint> indices, std::vector<Texture> textures);
	void Draw(Shader shader);
}

// Mesh Constructor //
Mesh(std::vector<Vertex> verticies, std::vector<GLuint> indices, std::vector<Texture> textures)
{
	//setting up the mesh data
	this->verticies = verticies;
	this->indices = indices;
	this->textures = textures;

	this-setupMesh();
}

void setupMesh()
{
	
}