#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>
#include <limits>
#include <queue>
#include <set>
#include <d3dx9.h>
#include <fbxsdk.h>
#include "D3D9.h"
#include "Model.h"
#include "Camera.h"
#include "GenerateLOD.h"
#ifndef _COMMON_H

extern std::string srcFbxName; // Resouce Fbx File Name

extern Model model_0;  //  Original Model
extern Model model_1;  //  LOD1
extern Model model_2;  //  LOD2
extern Model model_3;  //  LOD3
extern D3D9 d3d9;      // d3d9
extern GenerateLOD generateLOD;

extern Model model;    // The current LOD, may be LOD1 or LOD2 or LOD3

struct CUSTOMVERTEX
{
	//D3DXVECTOR3 position; //the position
	float x, y, z;
	float tu, tv;         //the texturre coordinates
};
extern DWORD D3DFVF_CUSTOMVERTEX;

#endif