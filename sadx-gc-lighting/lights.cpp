#include "stdafx.h"
#include <cstring>
#include <ninja.h>
#include "ShaderParameter.h"
#include "lights.h"

bool StageLight::operator==(const StageLight& rhs) const
{
	return !memcmp(&direction, &rhs.direction, sizeof(NJS_VECTOR))
		&& specular == rhs.specular
		&& multiplier == rhs.multiplier
		&& !memcmp(&diffuse, &rhs.diffuse, sizeof(NJS_VECTOR))
		&& !memcmp(&ambient, &rhs.ambient, sizeof(NJS_VECTOR));
}

bool StageLight::operator!=(const StageLight& rhs) const
{
	return !(*this == rhs);
}

bool StageLights::operator==(const StageLights& rhs) const
{
	return lights[0] == rhs.lights[0]
		&& lights[1] == rhs.lights[1]
		&& lights[2] == rhs.lights[2]
		&& lights[3] == rhs.lights[3];
}

bool StageLights::operator!=(const StageLights& rhs) const
{
	return !(*this == rhs);
}

template<>
bool ShaderParameter<StageLights>::commit(IDirect3DDevice9* device)
{
	if (is_modified())
	{
		device->SetVertexShaderConstantF(index, reinterpret_cast<float*>(&current), 16);
		device->SetPixelShaderConstantF(index, reinterpret_cast<float*>(&current), 16);

		clear();
		return true;
	}

	return false;
}
