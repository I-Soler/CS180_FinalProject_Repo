/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated September 24, 2021. Replaces all prior versions.
 *
 * Copyright (c) 2013-2021, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include <spine/spine.h>

#include <AEX.h>

using namespace AEX;

// include opengl
#include <Graphics/AEXGL.h>


namespace spine {
	CustomTextureLoader sTextureLoader;

	TextureLoader::TextureLoader() {
	}

	TextureLoader::~TextureLoader() {
	}
	void CustomTextureLoader::load(spine::AtlasPage& page, const spine::String& path)
	{

		// here you should create a texture resource (assuming you have a resource manager). 
		FilePath fp(path.buffer());
		//TResource<Texture>* newTex = aexResources->GetResource<Texture>(path.buffer(), false);
		TResource<Texture>* newTex = aexResources->GetResource<Texture>((fp.mFilename + fp.mExtension).c_str());

		// sanity check and error message
		if (!newTex) {
			DebugPrint("ERROR! Couldn't load Spine atlas page %s \n", path.buffer());
			return;
		}
		Texture* tex = newTex->get();
		// set texture filter based on atlas page setting
		{
			tex->Bind();
			switch (page.magFilter) {
			case spine::TextureFilter::TextureFilter_Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case spine::TextureFilter::TextureFilter_MipMapNearestNearest:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			}
			switch (page.minFilter) {
			case spine::TextureFilter::TextureFilter_Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				break;
			case spine::TextureFilter::TextureFilter_MipMapNearestNearest:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;
			}
			tex->Unbind();
		}

		// pass the texture pointer to the page. Spine doesn't actually do anything with it, 
		// it will be passed back to us when drawing the skeleton. 
		page.setRendererObject(newTex);

		// set the texture width and height
		page.width = tex->GetWidth();
		page.height = tex->GetHeight();
	}
	void CustomTextureLoader::unload(void* texture)
	{
		// without resource management:
		std::string texName = ((Texture*)texture)->GetName();
		aexResources->FreeResource(texName.c_str());	//MARTA: puede estar mal esta linea.
		// with resource management
		// ignore this call, let the resource manager take over. 
	}
}// namespace spine
