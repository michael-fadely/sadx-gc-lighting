#include "stdafx.h"
#include <d3d9.h>

// Mod loader
#include <SADXModLoader.h>
#include <Trampoline.h>

// MinHook
#include <MinHook.h>

// Local
#include "d3d.h"
#include "datapointers.h"
#include "globals.h"

static Trampoline* Direct3D_ParseMaterial_t        = nullptr;
static Trampoline* DrawLandTable_t                 = nullptr;

DataPointer(PaletteLight, LSPalette, 0x03ABDAF0);
DataPointer(NJS_VECTOR, NormalScaleMultiplier, 0x03B121F8);

static void update_material(const D3DMATERIAL9& material)
{
	using namespace d3d;

	if (!shaders_not_null())
	{
		return;
	}

	D3DMATERIALCOLORSOURCE colorsource;
	device->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, reinterpret_cast<DWORD*>(&colorsource));

	param::DiffuseSource    = colorsource;
	param::MaterialDiffuse  = material.Diffuse;
	param::MaterialSpecular = material.Specular;
	param::MaterialPower    = material.Power;
}

static void __cdecl CorrectMaterial_r()
{
	using namespace d3d;

	D3DMATERIAL9 material;
	device->GetMaterial(&material);

	material.Power = LSPalette.SP_pow;
	material.Ambient.r /= 255.0f;
	material.Ambient.g /= 255.0f;
	material.Ambient.b /= 255.0f;
	material.Ambient.a /= 255.0f;
	material.Diffuse.r /= 255.0f;
	material.Diffuse.g /= 255.0f;
	material.Diffuse.b /= 255.0f;
	material.Diffuse.a /= 255.0f;
	material.Specular.r /= 255.0f;
	material.Specular.g /= 255.0f;
	material.Specular.b /= 255.0f;
	material.Specular.a /= 255.0f;

	update_material(material);
	device->SetMaterial(&material);
}

static void __fastcall Direct3D_ParseMaterial_r(NJS_MATERIAL* material)
{
	using namespace d3d;

	TARGET_DYNAMIC(Direct3D_ParseMaterial)(material);

	if (!shaders_not_null())
	{
		return;
	}

	do_effect = false;

#ifdef _DEBUG
	const auto pad = ControllerPointers[0];
	if (pad && pad->HeldButtons & Buttons_Z)
	{
		return;
	}
#endif

	Uint32 flags = material->attrflags;

	if (_nj_control_3d_flag_ & NJD_CONTROL_3D_CONSTANT_ATTR)
	{
		flags = _nj_constant_attr_or_ | _nj_constant_attr_and_ & flags;
	}

	set_flags(ShaderFlags_Texture, (flags & NJD_FLAG_USE_TEXTURE) != 0);
	set_flags(ShaderFlags_Alpha, (flags & NJD_FLAG_USE_ALPHA) != 0);
	set_flags(ShaderFlags_EnvMap, (flags & NJD_FLAG_USE_ENV) != 0);
	set_flags(ShaderFlags_Light, (flags & NJD_FLAG_IGNORE_LIGHT) == 0);

	// Environment map matrix
	param::TextureTransform = *reinterpret_cast<D3DXMATRIX*>(0x038A5DD0);

	D3DMATERIAL9 mat;
	device->GetMaterial(&mat);
	update_material(mat);

	do_effect = true;
}

static void __cdecl DrawLandTable_r()
{
	const auto flag = _nj_control_3d_flag_;
	const auto or = _nj_constant_attr_or_;

	_nj_control_3d_flag_ |= NJD_CONTROL_3D_CONSTANT_ATTR;
	_nj_constant_attr_or_ |= NJD_FLAG_IGNORE_SPECULAR;

	TARGET_DYNAMIC(DrawLandTable)();

	_nj_control_3d_flag_ = flag;
	_nj_constant_attr_or_ = or;
}

static void __cdecl NormalScale_r(float x, float y, float z)
{
	if (x > FLT_EPSILON || y > FLT_EPSILON || z > FLT_EPSILON)
	{
		param::NormalScale = D3DXVECTOR3(x, y, z);
	}
	else
	{
		param::NormalScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}
}

extern "C"
{
	EXPORT ModInfo SADXModInfo = { ModLoaderVer };
	EXPORT void __cdecl Init(const char *path)
	{
		const auto handle = GetModuleHandle(L"d3d9.dll");

		if (handle == nullptr)
		{
			MessageBoxA(WindowHandle, "Unable to detect Direct3D 9 DLL. The mod will not function.",
				"D3D9 not loaded", MB_OK | MB_ICONERROR);

			return;
		}

		MH_Initialize();

		globals::mod_path    = path;
		globals::system_path = globals::mod_path + "\\system\\";
		globals::cache_path  = globals::mod_path + "\\cache\\";
		globals::shader_path = globals::system_path + "shader.hlsl";

		d3d::init_trampolines();

		Direct3D_ParseMaterial_t        = new Trampoline(0x00784850, 0x00784858, Direct3D_ParseMaterial_r);
		DrawLandTable_t                 = new Trampoline(0x0043A6A0, 0x0043A6A8, DrawLandTable_r);

		// Material callback hijack
		WriteJump(reinterpret_cast<void*>(0x0040A340), CorrectMaterial_r);

		// Vertex normal correction for certain objects in
		// Red Mountain and Sky Deck.
		WriteCall(reinterpret_cast<void*>(0x00411EDA), NormalScale_r);
		WriteCall(reinterpret_cast<void*>(0x00411F1D), NormalScale_r);
		WriteCall(reinterpret_cast<void*>(0x00411F44), NormalScale_r);
		WriteCall(reinterpret_cast<void*>(0x00412783), NormalScale_r);

		NormalScaleMultiplier = { 1.0f, 1.0f, 1.0f };
	}

#ifdef _DEBUG
	EXPORT void __cdecl OnFrame()
	{
		const auto pad = ControllerPointers[0];

		if (pad)
		{
			const auto pressed = pad->PressedButtons;
			if (pressed & Buttons_C)
			{
				d3d::load_shader();
			}

			if (pressed & Buttons_Up)
			{
				globals::particle_scale += 0.5f;
			}
			else if (pressed & Buttons_Down)
			{
				globals::particle_scale -= 0.5f;
			}
		}

		DisplayDebugStringFormatted(NJM_LOCATION(10, 10), "PARTICLE SCALE: %f", globals::particle_scale);
	}
#endif
}
