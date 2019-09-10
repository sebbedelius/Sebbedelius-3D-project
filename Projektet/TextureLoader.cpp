#include "TextureLoader.h"

TextureLoader::TextureLoader()
{
	CoInitialize(NULL);
}

TextureLoader::~TextureLoader()
{	
	CoUninitialize();		
}

HRESULT TextureLoader::loadFromFile(ID3D11Device * inGDevice, const wchar_t * filename, size_t maxsize)
{
	HRESULT hr = 0;

	hr = DirectX::CreateWICTextureFromFile(inGDevice, filename, nullptr,
		&this->textureRV, maxsize);

	if (FAILED(hr))
	{
		return hr;
	}

}

ID3D11ShaderResourceView* TextureLoader::getTextureRV() const
{
	return this->textureRV; //Be sure to release outside
}
