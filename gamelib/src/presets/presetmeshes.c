#include "presets/presetmeshes.h"
#include "presets/presetnames.h"

// The stock raylib function creates an XZ plane when we want an XY plane.
// Rather than faff about with re-uploading the vertex data into whatever
// the correct buffer is, we just create a mesh manually here.
static Mesh CreateQuadMesh()
{
	Mesh mesh = { 0 };

	// Vertices definition
	Vector3* vertices = (Vector3*)GameAlloc(4 * sizeof(Vector3));

	vertices[0] = (Vector3){ -0.5f, -0.5f, 0.0f };
	vertices[1] = (Vector3){  0.5f, -0.5f, 0.0f };
	vertices[2] = (Vector3){ -0.5f,  0.5f, 0.0f };
	vertices[3] = (Vector3){  0.5f,  0.5f, 0.0f };

	// Normals definition
	Vector3* normals = (Vector3*)GameAlloc(4 * sizeof(Vector3));

	normals[0] = (Vector3){ 0.0f, 0.0f, 1.0f };
	normals[1] = (Vector3){ 0.0f, 0.0f, 1.0f };
	normals[2] = (Vector3){ 0.0f, 0.0f, 1.0f };
	normals[3] = (Vector3){ 0.0f, 0.0f, 1.0f };

	// TexCoords definition
	Vector2* texCoOrds = (Vector2*)GameAlloc(4 * sizeof(Vector2));

	texCoOrds[0] = (Vector2){ 0.0f, 0.0f };
	texCoOrds[1] = (Vector2){ 1.0f, 0.0f };
	texCoOrds[2] = (Vector2){ 0.0f, 1.0f };
	texCoOrds[3] = (Vector2){ 1.0f, 1.0f };

	// Triangles definition (indices)
	int* triangles = (int*)GameAlloc(6 * sizeof(int));

	triangles[0] = 2;
	triangles[1] = 1;
	triangles[2] = 0;
	triangles[3] = 2;
	triangles[4] = 3;
	triangles[5] = 1;

	mesh.vertexCount = 4;
	mesh.triangleCount = 2;

	mesh.vertices = (float*)GameAlloc(mesh.vertexCount * 3 * sizeof(float));
	mesh.texcoords = (float*)GameAlloc(mesh.vertexCount * 2 * sizeof(float));
	mesh.normals = (float*)GameAlloc(mesh.vertexCount * 3 * sizeof(float));
	mesh.indices = (unsigned short*)GameAlloc(mesh.triangleCount * 3 * sizeof(unsigned short));

	// Mesh vertices position array
	for (int i = 0; i < mesh.vertexCount; i++)
	{
		mesh.vertices[3*i] = vertices[i].x;
		mesh.vertices[3*i + 1] = vertices[i].y;
		mesh.vertices[3*i + 2] = vertices[i].z;
	}

	// Mesh texcoords array
	for (int i = 0; i < mesh.vertexCount; i++)
	{
		mesh.texcoords[2*i] = texCoOrds[i].x;
		mesh.texcoords[2*i + 1] = texCoOrds[i].y;
	}

	// Mesh normals array
	for (int i = 0; i < mesh.vertexCount; i++)
	{
		mesh.normals[3*i] = normals[i].x;
		mesh.normals[3*i + 1] = normals[i].y;
		mesh.normals[3*i + 2] = normals[i].z;
	}

	// Mesh indices array initialization
	for (int i = 0; i < mesh.triangleCount * 3; i++)
	{
		mesh.indices[i] = (unsigned short)triangles[i];
	}

	GameFree(vertices);
	GameFree(normals);
	GameFree(texCoOrds);
	GameFree(triangles);

	UploadMesh(&mesh, false);

	return mesh;
}

Mesh PresetMeshes_Create(const char* presetName)
{
	if ( TextIsEqual(presetName, PRESET_MESH_QUAD) )
	{
		return CreateQuadMesh();
	}

	return (Mesh){ 0 };
}
