#include "utils.h"

bool dx4s::tools::ShaderCompile(const char *Filename, const char *entryPoint, const char *proFile, ID3DBlob **buffer, UINT Flag /* = D3DCOMPILE_ENABLE_STRICTNESS */) {
#if defined(_DEBUG) || defined(DEBUG)
	Flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *errBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(Filename, NULL, NULL, entryPoint, proFile, Flag, NULL, NULL, buffer, &errBuffer, NULL))) {
		if (errBuffer) {
			OutputDebugString((LPCSTR)errBuffer->GetBufferPointer());
			errBuffer->Release();
		}
		return false;
	}
	if (errBuffer)
		errBuffer->Release();
	return true;
}

bool dx4s::tools::ShaderCompileFromString(const char *SrcString, const char *entryPoint, const char *proFile, ID3DBlob **buffer, UINT Flag /*= D3DCOMPILE_ENABLE_STRICTNESS*/) {
#if defined(_DEBUG) || defined(DEBUG)
	Flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *errBuffer = nullptr;
	if (FAILED(D3DX11CompileFromMemory(SrcString, strlen(SrcString), NULL, NULL, NULL, entryPoint, proFile, Flag, NULL, NULL, buffer, &errBuffer, NULL))) {
		if (errBuffer) {
			OutputDebugString((LPCSTR)errBuffer->GetBufferPointer());
			errBuffer->Release();
		}
		return false;
	}
	if (errBuffer)
		errBuffer->Release();
	return true;
}
