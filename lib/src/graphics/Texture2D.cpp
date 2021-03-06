/*
* Viry3D
* Copyright 2014-2018 by Stack - stackos@qq.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "Texture2D.h"
#include "Image.h"
#include "io/File.h"
#include "memory/Memory.h"

namespace Viry3D
{
	bool Texture2D::LoadImageData(const ByteBuffer& buffer, ByteBuffer& colors, int& width, int& height, TextureFormat& format)
	{
		const byte JPG_HEAD[] = { 0xff, 0xd8, 0xff };
		const byte PNG_HEAD[] = { 0x89, 0x50, 0x4e, 0x47 };

		width = 0;
		height = 0;

		int bpp = 0;
		if (Memory::Compare(buffer.Bytes(), (void*) JPG_HEAD, 3) == 0)
		{
			colors = Image::LoadJPEG(buffer, width, height, bpp);
		}
		else if (Memory::Compare(buffer.Bytes(), (void*) PNG_HEAD, 4) == 0)
		{
			colors = Image::LoadPNG(buffer, width, height, bpp);
		}
		else
		{
			assert(!"invalid image file format");
			return false;
		}

		if (bpp == 32)
		{
			format = TextureFormat::RGBA32;
		}
		else if (bpp == 24)
		{
			format = TextureFormat::RGB24;
		}
		else if (bpp == 8)
		{
			format = TextureFormat::R8;
		}
		else
		{
			assert(!"invalid image file bpp");
			return false;
		}

		return true;
	}

	Ref<Texture2D> Texture2D::LoadFromData(const ByteBuffer& buffer,
		TextureWrapMode wrap_mode,
		FilterMode filter_mode,
		bool mipmap)
	{
		Ref<Texture2D> texture;

		int width;
		int height;
		ByteBuffer colors;
		TextureFormat format;

		if (Texture2D::LoadImageData(buffer, colors, width, height, format))
		{
			texture = Create(width, height, format, wrap_mode, filter_mode, mipmap, colors);
		}

		return texture;
	}

	Ref<Texture2D> Texture2D::LoadFromFile(const String& file,
		TextureWrapMode wrap_mode,
		FilterMode filter_mode,
		bool mipmap)
	{
		Ref<Texture2D> texture;

		if (File::Exist(file))
		{
			auto bytes = File::ReadAllBytes(file);

			texture = LoadFromData(bytes, wrap_mode, filter_mode, mipmap);
		}

		return texture;
	}

	Ref<Texture2D> Texture2D::Create(
		int width,
		int height,
		TextureFormat format,
		TextureWrapMode wrap_mode,
		FilterMode filter_mode,
		bool mipmap,
		const ByteBuffer& colors)
	{
		Ref<Texture2D> texture = Ref<Texture2D>(new Texture2D());
		texture->SetWidth(width);
		texture->SetHeight(height);
		texture->SetWrapMode(wrap_mode);
		texture->SetFilterMode(filter_mode);
		texture->m_mipmap = mipmap;
		texture->m_format = format;
		texture->m_colors = colors;

		texture->CreateTexture2D();

		return texture;
	}

	Ref<Texture2D> Texture2D::CreateExternalTexture(int width, int height, TextureFormat format, bool mipmap, void* external_texture)
	{
		Ref<Texture2D> texture = Ref<Texture2D>(new Texture2D());
		texture->SetWidth(width);
		texture->SetHeight(height);
		texture->SetWrapMode(TextureWrapMode::Clamp);
		texture->SetFilterMode(FilterMode::Bilinear);
		texture->m_mipmap = mipmap;
		texture->m_format = format;

		texture->SetExternalTexture2D(external_texture);
		texture->UpdateSampler();

		return texture;
	}

	Texture2D::Texture2D():
		m_format(TextureFormat::RGBA32)
	{
		SetName("Texture2D");
	}

	void Texture2D::EncodeToPNG(const String& file)
	{
		int bpp;
		auto format = this->GetFormat();
		switch (format)
		{
			case TextureFormat::RGBA32:
				bpp = 32;
				break;
			case TextureFormat::RGB24:
				bpp = 24;
				break;
			case TextureFormat::R8:
				bpp = 8;
				break;
			default:
				bpp = 0;
				break;
		}

		Image::EncodeToPNG(this, bpp, file);
	}
    
    void Texture2D::UpdateExternalTexture(void* external_texture)
    {
        this->SetExternalTexture2D(external_texture);
    }

	void Texture2D::UpdateTexture(int x, int y, int w, int h, const ByteBuffer& colors)
	{
		this->UpdateTexture2D(x, y, w, h, colors);
	}
}
