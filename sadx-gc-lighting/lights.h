#pragma once

struct StageLight
{
	NJS_VECTOR direction;
	float specular;
	float multiplier;
	NJS_VECTOR diffuse;
	NJS_VECTOR ambient;
	float padding[5];

	bool operator==(const StageLight& rhs) const;
	bool operator!=(const StageLight& rhs) const;
};

struct StageLights
{
	StageLight lights[4] {};

	bool operator==(const StageLights& rhs) const;
	bool operator!=(const StageLights& rhs) const;
};
