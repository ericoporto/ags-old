//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "gfx/gfxdriverfactory.h"
#include "gfx/ali3d_sdl_renderer.h"
#include "gfx/gfxfilter_allegro.h"
#include "gfx/ogl_support.h"
#include "debug/out.h"

#if AGS_OPENGL_DRIVER
#include "gfx/ali3dogl.h"
#include "gfx/gfxfilter_ogl.h"
#endif

#if AGS_HAS_DIRECT3D
#include "platform/windows/gfx/ali3dd3d.h"
#include "gfx/gfxfilter_d3d.h"
#endif

#include "main/main_allegro.h"

using namespace AGS::Common;

namespace AGS
{
namespace Engine
{

void GetGfxDriverFactoryNames(StringV &ids)
{
#if AGS_HAS_DIRECT3D
    ids.push_back("D3D9");
#endif
#if AGS_OPENGL_DRIVER
    ids.push_back("OGL");
#endif
    ids.push_back("Software");
}

IGfxDriverFactory *GetGfxDriverFactory(const String id)
{
    Debug::Printf("GetGfxDriverFactory id: %s", id.GetCStr());
#if AGS_HAS_DIRECT3D
    if (id.CompareNoCase("D3D9") == 0)
        return D3D::D3DGraphicsFactory::GetFactory();
#endif
#if AGS_OPENGL_DRIVER
    if (id.CompareNoCase("OGL") == 0 || id.CompareNoCase("D3D9") == 0)
        return OGL::OGLGraphicsFactory::GetFactory();
#endif
    //if ((id.CompareNoCase("Software") == 0) || (id.CompareNoCase("DX5") == 0)) {
#if AGS_PLATFORM_OS_EMSCRIPTEN
    return SDLRenderer::SDLRendererGraphicsFactory::GetFactory();
#endif
    if(id.CompareNoCase("OGL") != 0 || id.CompareNoCase("D3D9") != 0) {
        return SDLRenderer::SDLRendererGraphicsFactory::GetFactory();
    }
    set_allegro_error("No graphics factory with such id: %s", id.GetCStr());
    return nullptr;
}

} // namespace Engine
} // namespace AGS
