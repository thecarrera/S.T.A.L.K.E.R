// Based on dlls/d3dx9_36/mesh.c from Wine
//
// Mesh operations specific to D3DX9.
// 
// Copyright (C) 2005 Henri Verbeet
// Copyright (C) 2006 Ivan Gyurdiev
// Copyright (C) 2009 David Adam
// Copyright (C) 2010 Tony Wasserka
// Copyright (C) 2011 Dylan Smith
// Copyright (C) 2011 Michael Mc Donnell
// Copyright (C) 2013 Christian Costa
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA

#include "stdafx.h"
#include "D3DXCompat.hpp"
#include "xrCore/Debug/dxerr.h"

namespace
{
struct D3DXVECTOR2
{
    FLOAT x, y;
};
struct D3DXVECTOR3
{
    FLOAT x, y, z;
};
struct D3DXVECTOR4
{
    FLOAT x, y, z, w;
};
struct D3DXFLOAT16
{
    WORD value;
};

const UINT D3DXDecltypeSize[] =
{
    /* D3DDECLTYPE_FLOAT1    */ sizeof(FLOAT),
    /* D3DDECLTYPE_FLOAT2    */ sizeof(D3DXVECTOR2),
    /* D3DDECLTYPE_FLOAT3    */ sizeof(D3DXVECTOR3),
    /* D3DDECLTYPE_FLOAT4    */ sizeof(D3DXVECTOR4),
    /* D3DDECLTYPE_D3DCOLOR  */ sizeof(D3DCOLOR),
    /* D3DDECLTYPE_UBYTE4    */ 4*sizeof(BYTE),
    /* D3DDECLTYPE_SHORT2    */ 2*sizeof(SHORT),
    /* D3DDECLTYPE_SHORT4    */ 4*sizeof(SHORT),
    /* D3DDECLTYPE_UBYTE4N   */ 4*sizeof(BYTE),
    /* D3DDECLTYPE_SHORT2N   */ 2*sizeof(SHORT),
    /* D3DDECLTYPE_SHORT4N   */ 4*sizeof(SHORT),
    /* D3DDECLTYPE_USHORT2N  */ 2*sizeof(USHORT),
    /* D3DDECLTYPE_USHORT4N  */ 4*sizeof(USHORT),
    /* D3DDECLTYPE_UDEC3     */ 4, // 3 * 10 bits + 2 padding
    /* D3DDECLTYPE_DEC3N     */ 4,
    /* D3DDECLTYPE_FLOAT16_2 */ 2*sizeof(D3DXFLOAT16),
    /* D3DDECLTYPE_FLOAT16_4 */ 4*sizeof(D3DXFLOAT16),
};

void AppendDeclElement(D3DVERTEXELEMENT9 *decl, UINT *index, UINT *offset,
    D3DDECLTYPE type, D3DDECLUSAGE usage, UINT usageIndex)
{
    decl[*index].Stream = 0;
    decl[*index].Offset = (WORD)*offset;
    decl[*index].Type = (BYTE)type;
    decl[*index].Method = D3DDECLMETHOD_DEFAULT;
    decl[*index].Usage = (BYTE)usage;
    decl[*index].UsageIndex = (BYTE)usageIndex;
    *offset += D3DXDecltypeSize[type];
    (*index)++;
}
}

HRESULT D3DXDeclaratorFromFVF(DWORD fvf, D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE])
{
    static const D3DVERTEXELEMENT9 endElement = D3DDECL_END();
    DWORD texCount = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
    UINT offset = 0, idx = 0;
    if (fvf&(D3DFVF_RESERVED0|D3DFVF_RESERVED2))
        return D3DERR_INVALIDCALL;
    if (fvf&D3DFVF_POSITION_MASK)
    {
        BOOL hasBlend = (fvf&D3DFVF_XYZB5)>=D3DFVF_XYZB1;
        DWORD blendCount = 1+(((fvf&D3DFVF_XYZB5)-D3DFVF_XYZB1) >> 1);
        BOOL hasBlendIndex = (fvf&D3DFVF_LASTBETA_D3DCOLOR) || (fvf&D3DFVF_LASTBETA_UBYTE4);
        if (hasBlendIndex)
            blendCount--;
        if ((fvf&D3DFVF_POSITION_MASK)==D3DFVF_XYZW || hasBlend && blendCount>4)
            return D3DERR_INVALIDCALL;
        if ((fvf&D3DFVF_POSITION_MASK)==D3DFVF_XYZRHW)
            AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_POSITIONT, 0);
        else
            AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION, 0);
        if (hasBlend)
        {
            switch (blendCount)
            {
            case 0:
                break;
            case 1:
                AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT1, D3DDECLUSAGE_BLENDWEIGHT, 0);
                break;
            case 2:
                AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_BLENDWEIGHT, 0);
                break;
            case 3:
                AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_BLENDWEIGHT, 0);
                break;
            case 4:
                AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_BLENDWEIGHT, 0);
                break;
            default:
#ifdef DEBUG
                Msg("! ERROR: Invalid blend count %u", blendCount);
#endif
                break;
            }
            if (hasBlendIndex)
            {
                if (fvf&D3DFVF_LASTBETA_UBYTE4)
                    AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_UBYTE4, D3DDECLUSAGE_BLENDINDICES, 0);
                else if (fvf&D3DFVF_LASTBETA_D3DCOLOR)
                    AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_BLENDINDICES, 0);
            }
        }
    }
    if (fvf&D3DFVF_NORMAL)
        AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_NORMAL, 0);
    if (fvf&D3DFVF_PSIZE)
        AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT1, D3DDECLUSAGE_PSIZE, 0);
    if (fvf&D3DFVF_DIFFUSE)
        AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 0);
    if (fvf&D3DFVF_SPECULAR)
        AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR, 1);
    for (DWORD i = 0; i<texCount; i++)
    {
        switch ((fvf >> (16+2*i)) & 0x03)
        {
        case D3DFVF_TEXTUREFORMAT1:
            AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT1, D3DDECLUSAGE_TEXCOORD, i);
            break;
        case D3DFVF_TEXTUREFORMAT2:
            AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, i);
            break;
        case D3DFVF_TEXTUREFORMAT3:
            AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_TEXCOORD, i);
            break;
        case D3DFVF_TEXTUREFORMAT4:
            AppendDeclElement(decl, &idx, &offset, D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_TEXCOORD, i);
            break;
        }
    }
    decl[idx] = endElement;
    return D3D_OK;
}

UINT D3DXGetDeclLength(const D3DVERTEXELEMENT9 *decl)
{
    const D3DVERTEXELEMENT9 *element;
    for (element = decl; element->Stream!=0xff; element++);
    return element-decl;
}

UINT D3DXGetDeclVertexSize(const D3DVERTEXELEMENT9 *decl, DWORD streamIndex)
{
    UINT size = 0;
    if (!decl)
        return 0;
    for (const D3DVERTEXELEMENT9 *element = decl; element->Stream!=0xff; element++)
    {
        if (element->Stream!=streamIndex)
            continue;
        if (element->Type >= sizeof(D3DXDecltypeSize)/sizeof(*D3DXDecltypeSize))
            continue;
        UINT typeSize = D3DXDecltypeSize[element->Type];
        if (element->Offset+typeSize > size)
            size = element->Offset + typeSize;
    }
    return size;
}
