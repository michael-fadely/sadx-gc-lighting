#pragma once

#include <d3d9.h>
#include <d3dx9effect.h>
#include <d3d8to9.hpp>
#include <ninja.h>

#include "ShaderParameter.h"

enum ShaderFlags
{
	ShaderFlags_None         = 0,
	ShaderFlags_Texture      = 0b1,
	ShaderFlags_EnvMap       = 0b10,
	ShaderFlags_Alpha        = 0b100,
	ShaderFlags_Light        = 0b1000,
	ShaderFlags_Fog          = 0b10000,
	ShaderFlags_DepthMap     = 0b100000,
	ShaderFlags_SoftParticle = 0b1000000,
	ShaderFlags_Mask         = 0b1111111,
	ShaderFlags_Count
};

namespace d3d
{
	extern IDirect3DDevice9* device;
	extern VertexShader vertex_shader;
	extern PixelShader pixel_shader;

	extern bool do_effect;
	void load_shader();
	void set_flags(Uint32 flags, bool add = true);
	bool shaders_not_null();
	void init_trampolines();
}

namespace param
{
	extern ShaderParameter<D3DXMATRIX> WorldMatrix;
	extern ShaderParameter<D3DXMATRIX> ViewMatrix;
	extern ShaderParameter<D3DXMATRIX> ProjectionMatrix;
	extern ShaderParameter<D3DXMATRIX> wvMatrixInvT;
	extern ShaderParameter<D3DXMATRIX> TextureTransform;

	extern ShaderParameter<int> FogMode;
	extern ShaderParameter<D3DXVECTOR3> FogConfig;
	extern ShaderParameter<D3DXCOLOR> FogColor;

	extern ShaderParameter<D3DXVECTOR3> LightDirection;
	extern ShaderParameter<D3DXVECTOR3> CameraPosition;
	extern ShaderParameter<int> DiffuseSource;
	extern ShaderParameter<D3DXCOLOR> MaterialDiffuse;
	extern ShaderParameter<D3DXVECTOR3> NormalScale;
	extern ShaderParameter<D3DXCOLOR> MaterialSpecular;
	extern ShaderParameter<float> MaterialPower;
	extern ShaderParameter<D3DXCOLOR> LightDiffuse;
	extern ShaderParameter<D3DXCOLOR> LightSpecular;
	extern ShaderParameter<D3DXCOLOR> LightAmbient;
	extern ShaderParameter<float> SourceBlend;
	extern ShaderParameter<float> DestinationBlend;
	extern ShaderParameter<float> ParticleScale;
	extern ShaderParameter<float> DepthOverride;
	extern ShaderParameter<float> DrawDistance;
	extern ShaderParameter<D3DXVECTOR4> ViewPort; // HACK: pretend it's vector2
}

// Same as in the mod loader except with d3d8to9 types.
#pragma pack(push, 1)
struct MeshSetBuffer
{
	NJS_MESHSET_SADX *Meshset;
	void* field_4;
	int FVF;
	Direct3DVertexBuffer8* VertexBuffer;
	int Size;
	Direct3DIndexBuffer8* IndexBuffer;
	D3DPRIMITIVETYPE PrimitiveType;
	int MinIndex;
	int NumVertecies;
	int StartIndex;
	int PrimitiveCount;
};

struct __declspec(align(2)) PolyBuff_RenderArgs
{
	Uint32 StartVertex;
	Uint32 PrimitiveCount;
	Uint32 CullMode;
	Uint32 d;
};

struct PolyBuff
{
	Direct3DVertexBuffer8 *pStreamData;
	Uint32 TotalSize;
	Uint32 CurrentSize;
	Uint32 Stride;
	Uint32 FVF;
	PolyBuff_RenderArgs *RenderArgs;
	Uint32 LockCount;
	const char *name;
	int i;
};
#pragma pack(pop)
