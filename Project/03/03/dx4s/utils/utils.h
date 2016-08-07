#pragma once
#include "../dx4s.h"

namespace dx4s {
	class utils {
	public:
		static bool ShaderCompile(const char *Filename,
								  const char *entryPoint,
								  const char *proFile,
								  ID3DBlob **buffer,
								  UINT Flag = D3DCOMPILE_ENABLE_STRICTNESS);
	};
}