#include "stdafx.h"

#include <string>
#include <ninja.h>

namespace globals
{
#ifdef _DEBUG
	NJS_VECTOR light_dir = {};
#endif

	Sint32 light_type       = 0;
	bool landtable_specular = false;
	bool object_vcolor      = true;
	bool first_material     = false;

	std::string mod_path;
	std::string system_path;
	std::string cache_path;
	std::string shader_path;
}
