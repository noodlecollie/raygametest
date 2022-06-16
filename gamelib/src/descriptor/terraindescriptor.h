#pragma once

typedef struct TerrainDescriptor TerrainDescriptor;

TerrainDescriptor* TerrainDescriptor_LoadFromJSON(const char* filePath);
void TerrainDescriptor_Destroy(TerrainDescriptor* descriptor);
