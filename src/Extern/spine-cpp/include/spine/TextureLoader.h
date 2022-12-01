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

#ifndef Spine_TextureLoader_h
#define Spine_TextureLoader_h

#include <spine/SpineObject.h>
#include <spine/SpineString.h>

namespace spine {
	class AtlasPage;

	class SP_API TextureLoader : public SpineObject {
	public:
		TextureLoader();

		virtual ~TextureLoader();

		virtual void load(AtlasPage &page, const String &path) = 0;

		virtual void unload(void *texture) = 0;
	};

	

	/*
	spine-cpp's Atlas class expects to be passed an instance of TextureLoader to load
	and create an engine specific texture representation for a single atlas page.
	The TextureLoader class has two methods, load to load a method for an atlas page
	given a path, and unload to dispose of the texture.

	The load function may store the texture in the atlas page via a call to AtlasPage::setRendererObject().
	This makes it easy later on to get the texture an attachment references via a region in an atlas page.

	The load method is also supposed to set the width and height of the AtlasPage in pixels according to the
	texture file loaded by the engine. This data is required to compute texture coordinates by spine-c.

	The load function's path parameter is the path to the page image file, relative to the .atlas file
	path passed to the Atlas constructor, or relative to the dir parameter of the second Atlas constructor
	that loads the atlas from meory.
	*/

	class CustomTextureLoader : public spine::TextureLoader
	{

		// load the texture
		virtual void load(spine::AtlasPage& page, const spine::String& path);

		virtual void unload(void* texture);
	};

	extern CustomTextureLoader sTextureLoader;
}

#endif /* Spine_TextureLoader_h */
