#include "utils.h"

bool dx4s::utils::ShaderCompile(const char *Filename, const char *entryPoint, const char *proFile, ID3DBlob **buffer, UINT Flag /* = D3DCOMPILE_ENABLE_STRICTNESS */) {
#if defined(_DEBUG) || defined(DEBUG)
	Flag |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *errBuffer = nullptr;
	HRESULT res = D3DX11CompileFromFile(Filename, NULL, NULL, entryPoint, proFile, Flag, NULL, NULL, buffer, &errBuffer, NULL);
		if (FAILED(res)) {
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
