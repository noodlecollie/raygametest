#include "presets/presetmeshes.h"
#include "presets/presetnames.h"

static Mesh CreateQuadMesh()
{
	Mesh quad = GenMeshPlane(1.0f, 1.0f, 1, 1);

	for ( int vertIndex = 0; vertIndex < quad.vertexCount; ++vertIndex )
	{
		Vector3* vertex = ((Vector3*)quad.vertices) + vertIndex;

		vertex->y = vertex->z;
		vertex->z = 0;
	}

	return quad;
}

Mesh PresetMeshes_Create(const char* presetName)
{
	if ( TextIsEqual(presetName, PRESET_MESH_QUAD) )
	{
		return CreateQuadMesh();
	}

	return (Mesh){ 0 };
}
