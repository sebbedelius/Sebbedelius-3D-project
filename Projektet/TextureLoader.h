#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class TextureLoader
{
private:
	ID3D11ShaderResourceView* textureRV;	
public:
	TextureLoader();
	~TextureLoader();

	HRESULT loadFromFile(ID3D11Device* inGDevice, const wchar_t* filename,
		size_t maxsize = 0);	

	ID3D11ShaderResourceView* getTextureRV() const;
};

#endif
