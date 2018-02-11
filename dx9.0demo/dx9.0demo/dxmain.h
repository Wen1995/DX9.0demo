#pragma once
#include "d3d9.h"

//-----------�����ʽ--------------------

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
	DWORD color;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct NormalTexVertex
{
	float x, y, z;
	float nx, ny, nz;	//����
	float u, v;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

