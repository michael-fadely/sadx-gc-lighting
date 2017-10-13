#pragma once

#include <string>
#include <ninja.h>

namespace globals
{
#ifdef _DEBUG
	extern NJS_VECTOR light_dir;
#endif

	extern Sint32 light_type;
	extern bool landtable_specular;
	extern bool object_vcolor;
	extern bool first_material;

	extern std::string mod_path;
	extern std::string system_path;
	extern std::string cache_path;
	extern std::string shader_path;
}
