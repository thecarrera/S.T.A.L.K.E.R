#pragma once
#include <d3d9types.h>

enum _MAX_FVF_DECL_SIZE
{
    MAX_FVF_DECL_SIZE = MAXD3DDECLLENGTH+1
};

HRESULT D3DXDeclaratorFromFVF(DWORD fvf, D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE]);
UINT D3DXGetDeclLength(const D3DVERTEXELEMENT9 *decl);
UINT D3DXGetDeclVertexSize(const D3DVERTEXELEMENT9 *decl, DWORD streamIndex);
