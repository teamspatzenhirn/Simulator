// OBJ Model Loader

// This was taken from https://github.com/Bly7/OBJ-Loader
// and adapted to fit the coding style used here.
// The implementation was also moved to a separate file.

#ifndef INC_2019_OBJLOADER_H
#define INC_2019_OBJLOADER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

// Print progress to console while loading (large models)
// #define OBJL_CONSOLE_OUTPUT

// Namespace: OBJL
//
// Description: The namespace that holds eveyrthing that
//	is needed and used for the OBJ Model Loader
namespace objl
{
// Structure: Vector2
//
// Description: A 2D Vector that Holds Positional Data
struct Vector2
{
	// Default Constructor
	Vector2();
	// Variable Set Constructor
	Vector2(float X_, float Y_);

	// Bool Equals Operator Overload
	bool operator==(const Vector2 &other) const;

	// Bool Not Equals Operator Overload
	bool operator!=(const Vector2 &other) const;

	// Addition Operator Overload
	Vector2 operator+(const Vector2 &right) const;

	// Subtraction Operator Overload
	Vector2 operator-(const Vector2 &right) const;

	// Float Multiplication Operator Overload
	Vector2 operator*(const float &other) const;

	// Positional Variables
	float X;
	float Y;
};

// Structure: Vector3
//
// Description: A 3D Vector that Holds Positional Data
struct Vector3
{
	// Default Constructor
	Vector3();

	// Variable Set Constructor
	Vector3(float X_, float Y_, float Z_);

	// Bool Equals Operator Overload
	bool operator==(const Vector3 &other) const;

	// Bool Not Equals Operator Overload
	bool operator!=(const Vector3 &other) const;

	// Addition Operator Overload
	Vector3 operator+(const Vector3 &right) const;

	// Subtraction Operator Overload
	Vector3 operator-(const Vector3 &right) const;

	// Float Multiplication Operator Overload
	Vector3 operator*(const float &other) const;

	// Positional Variables
	float X;
	float Y;
	float Z;
};

// Structure: Vertex
//
// Description: Model Vertex object that holds
//	a Position, Normal, and Texture Coordinate
struct Vertex
{
	// Position Vector
	Vector3 Position;

	// Normal Vector
	Vector3 Normal;

	// Texture Coordinate Vector
	Vector2 TextureCoordinate;
};

struct Material
{
	Material();

	// Material Name
	std::string name;
	// Ambient Color
	Vector3 Ka;
	// Diffuse Color
	Vector3 Kd;
	// Specular Color
	Vector3 Ks;
	// Specular Exponent
	float Ns;
	// Optical Density
	float Ni;
	// Dissolve
	float d;
	// Illumination
	int illum;
	// Ambient Texture Map
	std::string map_Ka;
	// Diffuse Texture Map
	std::string map_Kd;
	// Specular Texture Map
	std::string map_Ks;
	// Specular Hightlight Map
	std::string map_Ns;
	// Alpha Texture Map
	std::string map_d;
	// Bump Map
	std::string map_bump;
};

// Structure: Mesh
//
// Description: A Simple Mesh Object that holds
//	a name, a vertex list, and an index list
struct Mesh
{
	// Default Constructor
	Mesh();

	// Variable Set Constructor
	Mesh(std::vector<Vector3> &_Positions,
		std::vector<Vector3> &_Normals,
		std::vector<Vector2> &_TCoords,
		std::vector<Vertex> &_Vertices,
		std::vector<unsigned int> &_Indices);

	// Position List
	std::vector<Vector3> Positions;
	// Normals List
	std::vector<Vector3> Normals;
	// Texture Coordinate List
	std::vector<Vector2> TCoords;

	// Mesh Name
	std::string MeshName;
	// Vertex List
	std::vector<Vertex> Vertices;
	// Index List
	std::vector<unsigned int> Indices;

	// Material
	Material MeshMaterial;
};

// Namespace: Math
//
// Description: The namespace that holds all of the math
//	functions need for OBJL
namespace math
{
// Vector3 Cross Product
Vector3 CrossV3(const Vector3 a, const Vector3 b);

// Vector3 Magnitude Calculation
float MagnitudeV3(const Vector3 in);

// Vector3 DotProduct
float DotV3(const Vector3 a, const Vector3 b);

// Angle between 2 Vector3 Objects
float AngleBetweenV3(const Vector3 a, const Vector3 b);
} // namespace math

// Namespace: Algorithm
//
// Description: The namespace that holds all of the
// Algorithms needed for OBJL
namespace algorithm
{
// Vector3 Multiplication Opertor Overload
Vector3 operator*(const float &left, const Vector3 &right);

// Check to see if a Vector3 Point is within a 3 Vector3 Triangle
bool inTriangle(Vector3 point, Vector3 tri1, Vector3 tri2, Vector3 tri3);

// Split a String into a string array at a given token
inline void split(const std::string &in,
				  std::vector<std::string> &out,
				  std::string token);

// Get tail of string after first token and possibly following spaces
inline std::string tail(const std::string &in);

// Get first token of string
inline std::string firstToken(const std::string &in);

// Get element at given index position
template <class T>
inline const T &getElement(const std::vector<T> &elements, std::string &index);

} // namespace algorithm

// Class: Loader
//
// Description: The OBJ Model Loader
class Loader
{
  public:
	// Default Constructor
	Loader();
	~Loader();

	// Load a file into the loader
	//
	// If file is loaded return true
	//
	// If the file is unable to be found
	// or unable to be loaded return false
	bool LoadFile(std::string Path);

	// Loaded Mesh Objects
	std::vector<Mesh> LoadedMeshes;
	// Loaded Vertex Objects
	std::vector<Vertex> LoadedVertices;
	// Loaded Index Positions
	std::vector<unsigned int> LoadedIndices;
	// Loaded Material Objects
	std::vector<Material> LoadedMaterials;

  private:
	// Generate vertices from a list of positions,
	//	tcoords, normals and a face line
	void GenVerticesFromRawOBJ(std::vector<Vertex> &oVerts,
							   const std::vector<Vector3> &iPositions,
							   const std::vector<Vector2> &iTCoords,
							   const std::vector<Vector3> &iNormals,
							   std::string icurline);

	// Triangulate a list of vertices into a face by printing
	//	inducies corresponding with triangles within it
	void VertexTriangluation(std::vector<unsigned int> &oIndices,
							 const std::vector<Vertex> &iVerts);

	// Load Materials from .mtl file
	bool LoadMaterials(std::string path);
};
} // namespace objl

#endif
