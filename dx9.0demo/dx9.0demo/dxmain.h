#pragma once
#include "d3d9.h"

//-----------顶点格式--------------------

struct XYZVertex
{
	float x, y, z;
	XYZVertex(float x, float y, float z) : x(x), y(y), z(z)
	{}
	XYZVertex() : x(0), y(0), z(0)
	{}
	static const DWORD FVF = D3DFVF_XYZ;
};

struct ColorVertex
{
	float x, y, z;
	D3DCOLOR color;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};


struct NormalVertex
{
	float x, y, z;
	float nx, ny, nz;
	NormalVertex(float x, float y, float z, float nx, float ny, float nz): x(x), y(y), z(z), nx(nx), ny(ny), nz(nz)
	{}
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
};

struct NormalTexVertex
{
	float x, y, z;
	float nx, ny, nz;	//法线
	float u, v;
	NormalTexVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) :x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), u(u), v(v)
	{}
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

