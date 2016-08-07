#pragma once
#include "../dx4s.h"
#include "sprite2d/sprite2d.h"

namespace dx4s {
	class tools {
	public:
		static bool ShaderCompile(const char *Filename,
								  const char *entryPoint,
								  const char *proFile,
								  ID3DBlob **buffer,
								  UINT Flag = D3DCOMPILE_ENABLE_STRICTNESS);

		static bool ShaderCompileFromString(const char *SrcString,
								  const char *entryPoint,
								  const char *proFile,
								  ID3DBlob **buffer,
								  UINT Flag = D3DCOMPILE_ENABLE_STRICTNESS);
	};
}