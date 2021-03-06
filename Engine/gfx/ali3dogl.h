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
//
// OpenGL graphics factory
//
//=============================================================================

#ifndef __AGS_EE_GFX__ALI3DOGL_H
#define __AGS_EE_GFX__ALI3DOGL_H

#include <memory>

#include "glm/glm.hpp"

#include "gfx/bitmap.h"
#include "gfx/ddb.h"
#include "gfx/gfxdriverfactorybase.h"
#include "gfx/gfxdriverbase.h"
#include "util/string.h"
#include "util/version.h"

#include "ogl_headers.h"

namespace AGS
{
namespace Engine
{

namespace OGL
{

using Common::Bitmap;
using Common::String;
using Common::Version;

typedef struct _OGLVECTOR {
    float x;
    float y;
} OGLVECTOR2D;


struct OGLCUSTOMVERTEX
{
    OGLVECTOR2D position;
    float tu;
    float tv;
};

struct OGLTextureTile : public TextureTile
{
    unsigned int texture;
};

class OGLBitmap : public BaseDDB
{
public:
    // Transparency is a bit counter-intuitive
    // 0=not transparent, 255=invisible, 1..254 barely visible .. mostly visible
    int  GetTransparency() const override { return _transparency; }
    void SetTransparency(int transparency) override { _transparency = transparency; }
    void SetFlippedLeftRight(bool isFlipped) override { _flipped = isFlipped; }
    void SetStretch(int width, int height, bool useResampler = true) override
    {
        _stretchToWidth = width;
        _stretchToHeight = height;
        _useResampler = useResampler;
    }
    // Rotation is set in degrees, clockwise
    void SetRotation(float degrees) override { _rotation = _rotation = -Common::Math::DegreesToRadians(degrees); }
    void SetLightLevel(int lightLevel) override  { _lightLevel = lightLevel; }
    void SetTint(int red, int green, int blue, int tintSaturation) override 
    {
        _red = red;
        _green = green;
        _blue = blue;
        _tintSaturation = tintSaturation;
    }
    void SetBlendMode(Common::BlendMode blendMode) override { _blendMode = blendMode; }

    // OpenGL texture data
    OGLCUSTOMVERTEX* _vertex;
    OGLTextureTile *_tiles;
    int _numTiles;

    // Drawing parameters
    bool _flipped;
    int _stretchToWidth, _stretchToHeight;
    float _rotation;
    bool _useResampler;
    int _red, _green, _blue;
    int _tintSaturation;
    int _lightLevel;
    bool _hasAlpha;
    int _transparency;
    Common::BlendMode _blendMode;

    OGLBitmap(int width, int height, int colDepth, bool opaque)
    {
        _vertex = nullptr;
        _tiles = nullptr;
        _numTiles = 0;

        _width = width;
        _height = height;
        _colDepth = colDepth;
        _flipped = false;
        _hasAlpha = false;
        _stretchToWidth = width;
        _stretchToHeight = height;
        _originX = _originY = 0.f;
        _useResampler = false;
        _rotation = 0;
        _red = _green = _blue = 0;
        _tintSaturation = 0;
        _lightLevel = 0;
        _transparency = 0;
        _opaque = opaque;
        _blendMode = Common::kBlend_Normal;
    }

    int GetWidthToRender() const { return _stretchToWidth; }
    int GetHeightToRender() const { return _stretchToHeight; }

    void Dispose();

    ~OGLBitmap() override
    {
        Dispose();
    }
};

typedef SpriteDrawListEntry<OGLBitmap> OGLDrawListEntry;
struct OGLSpriteBatch
{
    // List of sprites to render
    std::vector<OGLDrawListEntry> List;
    // Clipping viewport
    Rect Viewport;
    // Transformation matrix, built from the batch description
    glm::mat4 Matrix;
};
typedef std::vector<OGLSpriteBatch>    OGLSpriteBatches;


class OGLDisplayModeList : public IGfxModeList
{
public:
    OGLDisplayModeList(const std::vector<DisplayMode> &modes)
        : _modes(modes)
    {
    }

    int GetModeCount() const override
    {
        return _modes.size();
    }

    bool GetMode(int index, DisplayMode &mode) const override
    {
        if (index >= 0 && (size_t)index < _modes.size())
        {
            mode = _modes[index];
            return true;
        }
        return false;
    }

private:
    std::vector<DisplayMode> _modes;
};

// Shader program and its variable references;
// the variables are rather specific for AGS use (sprite tinting).
struct ShaderProgram
{
    GLuint Program = 0;
    GLuint Arg[4] {};
    // GLuint SamplerVar;      // texture ID
    // GLuint ColorVar;        // primary operation variable
    // GLuint AuxVar;          // auxiliary variable

    GLuint MVPMatrix = -1;

    GLuint TextureId = -1;

    GLuint Alpha = -1;

    GLuint TintHSV = -1;
    GLuint TintAmount = -1;
    GLuint TintLuminance = -1;

    GLuint LightingAmount = -1;
};

class OGLGfxFilter;

class OGLGraphicsDriver : public VideoMemoryGraphicsDriver
{
public:
    const char*GetDriverName() override { return "OpenGL"; }
    const char*GetDriverID() override { return "OGL"; }
    void SetTintMethod(TintMethod method) override;
    bool SetDisplayMode(const DisplayMode &mode) override;
    void UpdateDeviceScreen(const Size &screen_size) override;
    bool SetNativeSize(const Size &src_size) override;
    bool SetRenderFrame(const Rect &dst_rect) override;
    int GetDisplayDepthForNativeDepth(int native_color_depth) const override;
    IGfxModeList *GetSupportedModeList(int color_depth) override;
    bool IsModeSupported(const DisplayMode &mode) override;
    PGfxFilter GetGraphicsFilter() const override;
    void UnInit();
    // Clears the screen rectangle. The coordinates are expected in the **native game resolution**.
    void ClearRectangle(int x1, int y1, int x2, int y2, RGB *colorToUse) override;
    int  GetCompatibleBitmapFormat(int color_depth) override;
    IDriverDependantBitmap* CreateDDB(int width, int height, int color_depth, bool opaque) override;
    void UpdateDDBFromBitmap(IDriverDependantBitmap* bitmapToUpdate, Bitmap *bitmap, bool hasAlpha) override;
    void DestroyDDB(IDriverDependantBitmap* bitmap) override;
    void DrawSprite(int x, int y, IDriverDependantBitmap* bitmap) override
         { DrawSprite(x, y, x, y, bitmap); }
    void DrawSprite(int ox, int oy, int ltx, int lty, IDriverDependantBitmap* bitmap) override;
    void RenderToBackBuffer() override;
    void Render() override;
    void Render(int xoff, int yoff, GlobalFlipType flip) override;
    bool GetCopyOfScreenIntoBitmap(Bitmap *destination, bool at_native_res, GraphicResolution *want_fmt) override;
    void EnableVsyncBeforeRender(bool enabled) override { }
    void Vsync() override;
    void RenderSpritesAtScreenResolution(bool enabled, int supersampling) override;
    void FadeOut(int speed, int targetColourRed, int targetColourGreen, int targetColourBlue) override;
    void FadeIn(int speed, PALETTE p, int targetColourRed, int targetColourGreen, int targetColourBlue) override;
    void BoxOutEffect(bool blackingOut, int speed, int delay) override;
    bool SupportsGammaControl() override;
    void SetGamma(int newGamma) override;
    void UseSmoothScaling(bool enabled) override { _smoothScaling = enabled; }
    bool RequiresFullRedrawEachFrame() override { return true; }
    bool HasAcceleratedTransform() override { return true; }
    void SetScreenFade(int red, int green, int blue) override;
    void SetScreenTint(int red, int green, int blue) override;

    typedef std::shared_ptr<OGLGfxFilter> POGLFilter;

    void SetGraphicsFilter(POGLFilter filter);

    OGLGraphicsDriver();
    ~OGLGraphicsDriver() override;

private:
    POGLFilter _filter {};


    bool _firstTimeInit;
    SDL_Window *_sdlWindow = nullptr;
    SDL_GLContext _sdlGlContext = nullptr;
    // Position of backbuffer texture in world space
    GLfloat _backbuffer_vertices[8] {};
    // Relative position of source image on the backbuffer texture,
    // in local coordinates
    GLfloat _backbuffer_texture_coordinates[8];
    OGLCUSTOMVERTEX defaultVertices[4];
    bool _smoothScaling;
    bool _legacyPixelShader;

    ShaderProgram _tintShader;
    ShaderProgram _lightShader;
    ShaderProgram _transparencyShader;

    int device_screen_physical_width;
    int device_screen_physical_height;

    // Viewport and scissor rect, in OpenGL screen coordinates (0,0 is at left-bottom)
    Rect _viewportRect {};

    // These two flags define whether driver can, and should (respectively)
    // render sprites to texture, and then texture to screen, as opposed to
    // rendering to screen directly. This is known as supersampling mode
    bool _can_render_to_texture {};
    bool _do_render_to_texture {};
    // Backbuffer texture multiplier, used to determine a size of texture
    // relative to the native game size.
    int _super_sampling {};
    unsigned int _backbuffer {};
    unsigned int _fbo {};
    // Size of the backbuffer drawing area, equals to native size
    // multiplied by _super_sampling
    Size _backRenderSize {};
    // Actual size of the backbuffer texture, created by OpenGL
    Size _backTextureSize {};

    OGLSpriteBatches _spriteBatches;
    // TODO: these draw list backups are needed only for the fade-in/out effects
    // find out if it's possible to reimplement these effects in main drawing routine.
    SpriteBatchDescs _backupBatchDescs;
    OGLSpriteBatches _backupBatches;

    void InitSpriteBatch(size_t index, const SpriteBatchDesc &desc) override;
    void ResetAllBatches() override;

    // Sets up GL objects not related to particular display mode
    void FirstTimeInit();
    // Initializes Gl rendering context
    bool InitGlScreen(const DisplayMode &mode);
    bool CreateWindowAndGlContext(const DisplayMode &mode);
    void DeleteWindowAndGlContext();
    // Sets up general rendering parameters
    void InitGlParams(const DisplayMode &mode);
    void SetupDefaultVertices();
    // Test if rendering to texture is supported
    void TestRenderToTexture();
    // Test if supersampling should be allowed with the current setup
    void TestSupersampling();
    // Create shader programs for sprite tinting and changing light level
    void CreateShaders();
    // Configure backbuffer texture, that is used in render-to-texture mode
    void SetupBackbufferTexture();
    void DeleteBackbufferTexture();
    void CreateDesktopScreen();
    // Unset parameters and release resources related to the display mode
    void ReleaseDisplayMode();
    void AdjustSizeToNearestSupportedByCard(int *width, int *height);
    void UpdateTextureRegion(OGLTextureTile *tile, Bitmap *bitmap, OGLBitmap *target, bool hasAlpha);
    void CreateVirtualScreen();
    void do_fade(bool fadingOut, int speed, int targetColourRed, int targetColourGreen, int targetColourBlue);
    void _renderSprite(const OGLDrawListEntry *entry, const glm::mat4 &projection, const glm::mat4 &matGlobal);
    void SetupViewport();
    // Converts rectangle in top->down coordinates into OpenGL's native bottom->up coordinates
    Rect ConvertTopDownRect(const Rect &top_down_rect, int surface_height);

    // Backup all draw lists in the temp storage
    void BackupDrawLists();
    // Restore draw lists from the temp storage
    void RestoreDrawLists();
    // Deletes draw list backups
    void ClearDrawBackups();
    void _render(bool clearDrawListAfterwards);
    void RenderSpriteBatches(const glm::mat4 &projection);
    void RenderSpriteBatch(const OGLSpriteBatch &batch, const glm::mat4 &projection);
    void _reDrawLastFrame();
};


class OGLGraphicsFactory : public GfxDriverFactoryBase<OGLGraphicsDriver, OGLGfxFilter>
{
public:
    ~OGLGraphicsFactory() override;

    size_t               GetFilterCount() const override;
    const GfxFilterInfo *GetFilterInfo(size_t index) const override;
    String               GetDefaultFilterID() const override;

    static OGLGraphicsFactory   *GetFactory();

private:
    OGLGraphicsDriver   *EnsureDriverCreated() override;
    OGLGfxFilter        *CreateFilter(const String &id) override;

    static OGLGraphicsFactory *_factory;
};

} // namespace OGL
} // namespace Engine
} // namespace AGS

#endif // __AGS_EE_GFX__ALI3DOGL_H
