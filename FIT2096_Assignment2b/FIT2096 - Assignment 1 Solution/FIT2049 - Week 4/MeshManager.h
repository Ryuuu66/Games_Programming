#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <d3d11.h>
#include <map>
#include <string>

#include "Mesh.h"

class MeshManager
{
private:
	typedef std::map<std::string, Mesh*> MeshMap;
	MeshMap m_meshMap;

public:
	MeshManager();
	~MeshManager();
	bool Load(Direct3D* renderer, const char* filename);
	Mesh* GetMesh(const char* filename);
	void ReleaseMesh(Mesh* mesh);
	void Release();

	// Weel four exercises
	bool LoadTriangle(Direct3D* renderer, const char* idnetifier);
	bool LoadSquare(Direct3D* renderer, const char* idnetifier);
	bool LoadAbstractArt(Direct3D* renderer, const char* idnetifier);
	bool LoadAbstractArt3D(Direct3D* renderer, const char* idnetifier);

};

#endif