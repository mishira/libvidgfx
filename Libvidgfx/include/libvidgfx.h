//*****************************************************************************
// Libvidgfx: A graphics library for video compositing
//
// Copyright (C) 2014 Lucas Murray <lucas@polyflare.com>
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//*****************************************************************************

#ifndef LIBVIDGFX_H
#define LIBVIDGFX_H

#include <QtCore/qglobal.h>

#ifdef Q_OS_WIN
#define VIDGFX_D3D_ENABLED 1
#else
#define VIDGFX_D3D_ENABLED 0
#endif
#if !VIDGFX_D3D_ENABLED
#error Unsupported system
#endif

#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QMatrix4x4>
#if VIDGFX_D3D_ENABLED
#include <windows.h>
#endif // VIDGFX_D3D_ENABLED

//#ifdef __cplusplus
//extern "C" {
//#endif

// Export symbols from the DLL while allowing header file reuse by users
#ifdef VIDGFX_LIB
#define API_EXPORT Q_DECL_EXPORT
#else
#define API_EXPORT Q_DECL_IMPORT
#endif

//=============================================================================
// Global application constants

// Library version. NOTE: Don't forget to update the values in the resource
// files as well ("Libvidgfx.rc")
#define VIDGFX_VER_STR "v0.6.0"
#define VIDGFX_VER_MAJOR 0
#define VIDGFX_VER_MINOR 6
#define VIDGFX_VER_PATCH 0

//=============================================================================
// Enumerations

// Do not use line topologies as they are unreliable and prone to bugs
enum VidgfxTopology {
	GfxTriangleListTopology,
	GfxTriangleStripTopology
};

enum VidgfxRendTarget {
	GfxScreenTarget = 0,
	GfxCanvas1Target,
	GfxCanvas2Target,
	GfxScratch1Target,
	GfxScratch2Target,
	GfxUserTarget // A dynamic target (E.g. scaler)
};

// WARNING: If this enum is modified `DShowRenderer::compareFormats()` and
// `D3DContext::convertToBgrx()` also need to be updated.
enum VidgfxPixFormat {
	GfxNoFormat = 0,

	// Uncompressed RGB with a single packed plane
	GfxRGB24Format, // Convert to RGB32 on CPU
	GfxRGB32Format, // DXGI_FORMAT_B8G8R8X8_UNORM
	GfxARGB32Format, // DXGI_FORMAT_B8G8R8A8_UNORM

	// YUV 4:2:0 formats with 3 separate planes
	GfxYV12Format, // NxM Y, (N/2)x(M/2) V, (N/2)x(M/2) U
	GfxIYUVFormat, // NxM Y, (N/2)x(M/2) U, (N/2)x(M/2) V (A.k.a. I420) (Logitech C920)

	// YUV 4:2:0 formats with 2 separate planes
	GfxNV12Format, // NxM Y, Nx(M/2) interleaved UV

	// YUV 4:2:2 formats with a single packed plane
	GfxUYVYFormat, // UYVY, "Most popular after YV12"
	GfxHDYCFormat, // UYVY with BT.709, Used by Blackmagic Design
	GfxYUY2Format, // YUYV (Microsoft HD-3000, MacBook Pro FaceTime HD)

	NUM_PIXEL_FORMAT_TYPES // Must be last
};
static const char * const VidgfxPixFormatStrs[] = {
	"Unknown",

	// Uncompressed RGB with a single packed plane
	"RGB24",
	"RGB32",
	"ARGB32",

	// YUV 4:2:0 formats with 3 separate planes
	"YV12",
	"IYUV",

	// YUV 4:2:0 formats with 2 separate planes
	"NV12",

	// YUV 4:2:2 formats with a single packed plane
	"UYVY",
	"HDYC",
	"YUY2"
};

enum VidgfxShader {
	GfxNoShader = 0,
	GfxSolidShader,
	GfxTexDecalShader,
	GfxTexDecalGbcsShader,
	GfxTexDecalRgbShader,
	GfxResizeLayerShader,
	GfxRgbNv16Shader,
	GfxYv12RgbShader,
	GfxUyvyRgbShader,
	GfxHdycRgbShader,
	GfxYuy2RgbShader
};

enum VidgfxFilter {
	// Standard filters shown to the user
	GfxPointFilter = 0,
	GfxBilinearFilter,
	//GfxBicubicFilter,

	NUM_STANDARD_TEXTURE_FILTERS, // Must be after all standard filters

	// Special internal filters
	GfxResizeLayerFilter = NUM_STANDARD_TEXTURE_FILTERS
};
static const char * const VidgfxFilterStrs[] = {
	"Nearest neighbour",
	"Bilinear",
	//"Bicubic",
};
static const char * const VidgfxFilterQualStrs[] = {
	"Low (Nearest neighbour)",
	"Medium (Bilinear)",
	//"High (Bicubic)",
};

enum VidgfxBlending {
	GfxNoBlending = 0,
	GfxAlphaBlending,
	GfxPremultipliedBlending
};

typedef int VidgfxTexFlags;
enum VidgfxTexFlags_ {
	GfxWritableFlag = (1 << 0),
	GfxTargetableFlag = (1 << 1),
	GfxStagingFlag = (1 << 2),
	GfxGDIFlag = (1 << 3) // Used by `D3DContext` only
};

enum VidgfxOrientation {
	GfxUnchangedOrient = 0,
	GfxFlippedOrient,
	GfxMirroredOrient,
	GfxFlippedMirroredOrient
};

enum VidgfxLogLvl {
	GfxNotice = 0,
	GfxWarning,
	GfxCritical
};

//=============================================================================
// C interface datatypes

#define DECLARE_OPAQUE(name) struct name { int unused; }
DECLARE_OPAQUE(VidgfxContext);
DECLARE_OPAQUE(VidgfxTex);
DECLARE_OPAQUE(VidgfxVertBuf);
DECLARE_OPAQUE(VidgfxTexDecalBuf);
DECLARE_OPAQUE(VidgfxD3DContext);
DECLARE_OPAQUE(VidgfxD3DTex);
#undef DECLARE_OPAQUE

//=============================================================================
// Library initialization

API_EXPORT bool	vidgfx_init__(
	int libVerMajor, int libVerMinor, int libVerPatch);

/// <summary>
/// Initializes Libvidgfx. Must be called as the very first thing in `main()`.
/// </summary>
#define vidgfx_init() \
	if(!vidgfx_init__( \
	VIDGFX_VER_MAJOR, VIDGFX_VER_MINOR, VIDGFX_VER_PATCH)) \
	return 1

//=============================================================================
// GfxLog C interface

typedef void VidgfxLogCallback(
	const QString &cat, const QString &msg, VidgfxLogLvl lvl);

API_EXPORT void vidgfx_set_log_callback(
	VidgfxLogCallback *callback);

//=============================================================================
// VertexBuffer C interface

//-----------------------------------------------------------------------------
// Methods

API_EXPORT float *vidgfx_vertbuf_get_data_ptr(
	VidgfxVertBuf *buf);
API_EXPORT int vidgfx_vertbuf_get_num_floats(
	VidgfxVertBuf *buf);

API_EXPORT void vidgfx_vertbuf_set_num_verts(
	VidgfxVertBuf *buf,
	int num_verts);
API_EXPORT int vidgfx_vertbuf_get_num_verts(
	VidgfxVertBuf *buf);
API_EXPORT void vidgfx_vertbuf_set_vert_size(
	VidgfxVertBuf *buf,
	int vert_size);
API_EXPORT int vidgfx_vertbuf_get_vert_size(
	VidgfxVertBuf *buf);

API_EXPORT void vidgfx_vertbuf_set_dirty(
	VidgfxVertBuf *buf,
	bool dirty = true);
API_EXPORT bool vidgfx_vertbuf_is_dirty(
	VidgfxVertBuf *buf);

//=============================================================================
// TexDecalVertBuf C interface

// Buffer information for `vidgfx_create_tex_decal_rect()`. 4 rectangles of 6
// vertices where each vertex is 8 floats.
#define VIDGFX_SCROLL_RECT_NUM_VERTS (4 * 6)
#define VIDGFX_SCROLL_RECT_NUM_FLOATS (VIDGFX_SCROLL_RECT_NUM_VERTS * 8)
#define VIDGFX_SCROLL_RECT_BUF_SIZE \
	(VIDGFX_SCROLL_RECT_NUM_FLOATS * sizeof(float))

//-----------------------------------------------------------------------------
// Constructor/destructor

API_EXPORT VidgfxTexDecalBuf *vidgfx_texdecalbuf_new(
	VidgfxContext *context = NULL);
API_EXPORT void vidgfx_texdecalbuf_destroy(
	VidgfxTexDecalBuf *buf);

//-----------------------------------------------------------------------------
// Methods

API_EXPORT void vidgfx_texdecalbuf_set_context(
	VidgfxTexDecalBuf *buf,
	VidgfxContext *context);
API_EXPORT VidgfxVertBuf *vidgfx_texdecalbuf_get_vert_buf(
	VidgfxTexDecalBuf *buf); // Applies settings
API_EXPORT VidgfxTopology vidgfx_texdecalbuf_get_topology(
	VidgfxTexDecalBuf *buf);
API_EXPORT void vidgfx_texdecalbuf_destroy_vert_buf(
	VidgfxTexDecalBuf *buf);

// Position
API_EXPORT void vidgfx_texdecalbuf_set_rect(
	VidgfxTexDecalBuf *buf,
	const QRectF &rect);
API_EXPORT QRectF vidgfx_texdecalbuf_get_rect(
	VidgfxTexDecalBuf *buf);

// Scrolling
API_EXPORT void vidgfx_texdecalbuf_scroll_by(
	VidgfxTexDecalBuf *buf,
	const QPointF &delta);
API_EXPORT void vidgfx_texdecalbuf_scroll_by(
	VidgfxTexDecalBuf *buf,
	float x_delta,
	float y_delta);
API_EXPORT void vidgfx_texdecalbuf_reset_scrolling(
	VidgfxTexDecalBuf *buf);
API_EXPORT void vidgfx_texdecalbuf_set_round_offset(
	VidgfxTexDecalBuf *buf,
	bool round);
API_EXPORT bool vidgfx_texdecalbuf_get_round_offset(
	VidgfxTexDecalBuf *buf);

// Texture UV
API_EXPORT void vidgfx_texdecalbuf_set_tex_uv(
	VidgfxTexDecalBuf *buf,
	const QPointF &top_left,
	const QPointF &top_right,
	const QPointF &bot_left,
	const QPointF &bot_right);
API_EXPORT void vidgfx_texdecalbuf_set_tex_uv(
	VidgfxTexDecalBuf *buf,
	const QRectF &norm_rect,
	VidgfxOrientation orient = GfxUnchangedOrient);
API_EXPORT void vidgfx_texdecalbuf_set_tex_uv(
	VidgfxTexDecalBuf *buf,
	const QPointF &top_left,
	const QPointF &bot_right,
	VidgfxOrientation orient = GfxUnchangedOrient);
API_EXPORT void vidgfx_texdecalbuf_get_tex_uv(
	VidgfxTexDecalBuf *buf,
	QPointF *top_left,
	QPointF *top_right,
	QPointF *bot_left,
	QPointF *bot_right);

//=============================================================================
// Texture C interface

//-----------------------------------------------------------------------------
// Methods

API_EXPORT bool vidgfx_tex_is_valid(
	VidgfxTex *tex);
API_EXPORT bool vidgfx_tex_is_mapped(
	VidgfxTex *tex);
API_EXPORT void *vidgfx_tex_get_data_ptr(
	VidgfxTex *tex);
API_EXPORT int vidgfx_tex_get_stride(
	VidgfxTex *tex);

API_EXPORT bool vidgfx_tex_is_writable(
	VidgfxTex *tex);
API_EXPORT bool vidgfx_tex_is_targetable(
	VidgfxTex *tex);
API_EXPORT bool vidgfx_tex_is_staging(
	VidgfxTex *tex);
API_EXPORT QSize vidgfx_tex_get_size(
	VidgfxTex *tex);
API_EXPORT int vidgfx_tex_get_width(
	VidgfxTex *tex);
API_EXPORT int vidgfx_tex_get_height(
	VidgfxTex *tex);

API_EXPORT void vidgfx_tex_update_data(
	VidgfxTex *tex,
	const QImage &img);

//-----------------------------------------------------------------------------
// Interface

API_EXPORT void *vidgfx_tex_map(
	VidgfxTex *tex);
API_EXPORT void vidgfx_tex_unmap(
	VidgfxTex *tex);

API_EXPORT bool vidgfx_tex_is_srgb_hack(
	VidgfxTex *tex);

//=============================================================================
// GraphicsContext C interface

// The number of vertices required to represent one line
#define VIDGFX_NUM_VERTS_PER_LINE (6)

// The number of vertices required to represent one rectangle
#define VIDGFX_NUM_VERTS_PER_RECT (4 * VIDGFX_NUM_VERTS_PER_LINE)

// Buffer information for `createSolidRect()` (1 vertex = 8 floats)
#define VIDGFX_SOLID_RECT_NUM_VERTS (4)
#define VIDGFX_SOLID_RECT_NUM_FLOATS (VIDGFX_SOLID_RECT_NUM_VERTS * 8)
#define VIDGFX_SOLID_RECT_BUF_SIZE \
	(VIDGFX_SOLID_RECT_NUM_FLOATS * sizeof(float))

// Buffer information for `createSolidRectOutline()` (1 vertex = 8 floats)
#define VIDGFX_SOLID_RECT_OUTLINE_NUM_VERTS (VIDGFX_NUM_VERTS_PER_RECT)
#define VIDGFX_SOLID_RECT_OUTLINE_NUM_FLOATS \
	(VIDGFX_SOLID_RECT_OUTLINE_NUM_VERTS * 8)
#define VIDGFX_SOLID_RECT_OUTLINE_BUF_SIZE \
	(VIDGFX_SOLID_RECT_OUTLINE_NUM_FLOATS * sizeof(float))

// Buffer information for `createTexDecalRect()` (1 vertex = 8 floats)
#define VIDGFX_TEX_DECAL_RECT_NUM_VERTS (4)
#define VIDGFX_TEX_DECAL_RECT_NUM_FLOATS (VIDGFX_TEX_DECAL_RECT_NUM_VERTS * 8)
#define VIDGFX_TEX_DECAL_RECT_BUF_SIZE \
	(VIDGFX_TEX_DECAL_RECT_NUM_FLOATS * sizeof(float))

// Buffer information for `createResizeRect()` (1 vertex = 4 floats)
#define VIDGFX_RESIZE_RECT_NUM_VERTS (10 * VIDGFX_NUM_VERTS_PER_RECT)
#define VIDGFX_RESIZE_RECT_NUM_FLOATS (VIDGFX_RESIZE_RECT_NUM_VERTS * 4)
#define VIDGFX_RESIZE_RECT_BUF_SIZE \
	(VIDGFX_RESIZE_RECT_NUM_FLOATS * sizeof(float))

//-----------------------------------------------------------------------------
// Static methods

API_EXPORT bool vidgfx_create_solid_rect(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	const QColor &col);
API_EXPORT bool vidgfx_create_solid_rect(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	const QColor &tl_col,
	const QColor &tr_col,
	const QColor &bl_col,
	const QColor &br_col);

API_EXPORT bool vidgfx_create_solid_rect_outline(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	const QColor &col,
	const QPointF &half_width = QPointF(0.5f, 0.5f));
API_EXPORT bool vidgfx_create_solid_rect_outline(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	const QColor &tl_col,
	const QColor &tr_col,
	const QColor &bl_col,
	const QColor &br_col,
	const QPointF &half_width = QPointF(0.5f, 0.5f));

API_EXPORT bool vidgfx_create_tex_decal_rect(
	VidgfxVertBuf *out_buf,
	const QRectF &rect);
API_EXPORT bool vidgfx_create_tex_decal_rect(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	const QPointF &br_uv);
API_EXPORT bool vidgfx_create_tex_decal_rect(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	const QPointF &tl_uv,
	const QPointF &tr_uv,
	const QPointF &bl_uv,
	const QPointF &br_uv);

API_EXPORT bool vidgfx_create_resize_rect(
	VidgfxVertBuf *out_buf,
	const QRectF &rect,
	float handle_size,
	const QPointF &half_width = QPointF(0.5f, 0.5f));

// Helpers
API_EXPORT quint32 vidgfx_next_pow_two(
	quint32 n);

//-----------------------------------------------------------------------------
// Methods

API_EXPORT void vidgfx_context_set_view_mat(
	VidgfxContext *context,
	const QMatrix4x4 &matrix);
API_EXPORT QMatrix4x4 vidgfx_context_get_view_mat(
	VidgfxContext *context);
API_EXPORT void vidgfx_context_set_proj_mat(
	VidgfxContext *context,
	const QMatrix4x4 &matrix);
API_EXPORT QMatrix4x4 vidgfx_context_get_proj_mat(
	VidgfxContext *context);

API_EXPORT void vidgfx_context_set_screen_view_mat(
	VidgfxContext *context,
	const QMatrix4x4 &matrix);
API_EXPORT QMatrix4x4 vidgfx_context_get_screen_view_mat(
	VidgfxContext *context);
API_EXPORT void vidgfx_context_set_screen_proj_mat(
	VidgfxContext *context,
	const QMatrix4x4 &matrix);
API_EXPORT QMatrix4x4 vidgfx_context_get_screen_proj_mat(
	VidgfxContext *context);

API_EXPORT void vidgfx_context_set_user_render_target(
	VidgfxContext *context,
	VidgfxTex *tex_a,
	VidgfxTex *tex_b = NULL);
API_EXPORT VidgfxTex *vidgfx_context_get_user_render_target(
	VidgfxContext *context,
	int index);
API_EXPORT void vidgfx_context_set_user_render_target_viewport(
	VidgfxContext *context,
	const QRect &rect);
API_EXPORT void vidgfx_context_set_user_render_target_viewport(
	VidgfxContext *context,
	const QSize &size);
API_EXPORT QRect vidgfx_context_get_user_render_target_viewport(
	VidgfxContext *context);

API_EXPORT void vidgfx_context_set_resize_layer_rect(
	VidgfxContext *context,
	const QRectF &rect);
API_EXPORT QRectF vidgfx_context_get_resize_layer_rect(
	VidgfxContext *context);

API_EXPORT void vidgfx_context_set_rgb_nv16_px_size(
	VidgfxContext *context,
	const QPointF &size);
API_EXPORT QPointF vidgfx_context_get_rgb_nv16_px_size(
	VidgfxContext *context);

API_EXPORT void vidgfx_context_set_tex_decal_mod_color(
	VidgfxContext *context,
	const QColor &color);
API_EXPORT QColor vidgfx_context_get_tex_decal_mod_color(
	VidgfxContext *context);

API_EXPORT void vidgfx_context_set_tex_decal_effects(
	VidgfxContext *context,
	float gamma,
	float brightness,
	float contrast,
	float saturation);
API_EXPORT bool vidgfx_context_set_tex_decal_effects_helper(
	VidgfxContext *context,
	float gamma,
	int brightness,
	int contrast,
	int saturation);
API_EXPORT const float *vidgfx_context_get_tex_decal_effects(
	VidgfxContext *context);

API_EXPORT bool vidgfx_context_dilute_img(
	VidgfxContext *context,
	QImage &img);

//-----------------------------------------------------------------------------
// Interface

API_EXPORT bool vidgfx_context_is_valid(
	VidgfxContext *context);
API_EXPORT void vidgfx_context_flush(
	VidgfxContext *context);

// Buffers
API_EXPORT VidgfxVertBuf *vidgfx_context_new_vertbuf(
	VidgfxContext *context,
	int size);
API_EXPORT void vidgfx_context_destroy_vertbuf(
	VidgfxContext *context,
	VidgfxVertBuf *buf);
API_EXPORT VidgfxTex *vidgfx_context_new_tex(
	VidgfxContext *context,
	QImage img,
	bool writable = false,
	bool targetable = false);
API_EXPORT VidgfxTex *vidgfx_context_new_tex(
	VidgfxContext *context,
	const QSize &size,
	bool writable = false,
	bool targetable = false,
	bool use_bgra = false);
API_EXPORT VidgfxTex *vidgfx_context_new_tex(
	VidgfxContext *context,
	const QSize &size,
	VidgfxTex *same_format,
	bool writable = false,
	bool targetable = false);
API_EXPORT VidgfxTex *vidgfx_context_new_staging_tex(
	VidgfxContext *context,
	const QSize &size);
API_EXPORT void vidgfx_context_destroy_tex(
	VidgfxContext *context,
	VidgfxTex *tex);
API_EXPORT bool vidgfx_context_copy_tex_data(
	VidgfxContext *context,
	VidgfxTex *dst,
	VidgfxTex *src,
	const QPoint &dst_pos,
	const QRect &src_rect);

// Render targets
API_EXPORT void vidgfx_context_resize_screen_target(
	VidgfxContext *context,
	const QSize &new_size);
API_EXPORT void vidgfx_context_resize_canvas_target(
	VidgfxContext *context,
	const QSize &new_size);
API_EXPORT void vidgfx_context_resize_scratch_target(
	VidgfxContext *context,
	const QSize &new_size);
API_EXPORT void vidgfx_context_swap_screen_bufs(
	VidgfxContext *context);
API_EXPORT VidgfxTex *vidgfx_context_get_target_tex(
	VidgfxContext *context,
	VidgfxRendTarget target);
API_EXPORT VidgfxRendTarget vidgfx_context_get_next_scratch_target(
	VidgfxContext *context);
API_EXPORT QPointF vidgfx_context_get_scratch_target_to_tex_ratio(
	VidgfxContext *context);

// Advanced rendering
API_EXPORT VidgfxTex *vidgfx_context_prepare_tex(
	VidgfxContext *context,
	VidgfxTex *tex,
	const QSize &size,
	VidgfxFilter filter,
	bool set_filter,
	QPointF &px_size_out,
	QPointF &bot_right_out);
API_EXPORT VidgfxTex *vidgfx_context_prepare_tex(
	VidgfxContext *context,
	VidgfxTex *tex,
	const QRect &crop_rect,
	const QSize &size,
	VidgfxFilter filter,
	bool set_filter,
	QPointF &px_size_out,
	QPointF &top_left_out,
	QPointF &bot_right_out);
API_EXPORT VidgfxTex *vidgfx_context_convert_to_bgrx(
	VidgfxContext *context,
	VidgfxPixFormat format,
	VidgfxTex *plane_a,
	VidgfxTex *plane_b,
	VidgfxTex *plane_c);

// Drawing
API_EXPORT void vidgfx_context_set_render_target(
	VidgfxContext *context,
	VidgfxRendTarget target);
API_EXPORT void vidgfx_context_set_shader(
	VidgfxContext *context,
	VidgfxShader shader);
API_EXPORT void vidgfx_context_set_topology(
	VidgfxContext *context,
	VidgfxTopology topology);
API_EXPORT void vidgfx_context_set_blending(
	VidgfxContext *context,
	VidgfxBlending blending);
API_EXPORT void vidgfx_context_set_tex(
	VidgfxContext *context,
	VidgfxTex *tex_a,
	VidgfxTex *tex_b = NULL,
	VidgfxTex *tex_c = NULL);
API_EXPORT void vidgfx_context_set_tex_filter(
	VidgfxContext *context,
	VidgfxFilter filter);
API_EXPORT void vidgfx_context_clear(
	VidgfxContext *context,
	const QColor &color);
API_EXPORT void vidgfx_context_draw_buf(
	VidgfxContext *context,
	VidgfxVertBuf *buf,
	int num_vertices = -1,
	int start_vertex = 0);

//-----------------------------------------------------------------------------
// Signals

typedef void VidgfxContextInitializedCallback(
	void *opaque, VidgfxContext *context);
typedef void VidgfxContextDestroyingCallback(
	void *opaque, VidgfxContext *context);

API_EXPORT void vidgfx_context_add_initialized_callback(
	VidgfxContext *context,
	VidgfxContextInitializedCallback *initialized,
	void *opaque);
API_EXPORT void vidgfx_context_remove_initialized_callback(
	VidgfxContext *context,
	VidgfxContextInitializedCallback *initialized,
	void *opaque);
API_EXPORT void vidgfx_context_add_destroying_callback(
	VidgfxContext *context,
	VidgfxContextDestroyingCallback *destroying,
	void *opaque);
API_EXPORT void vidgfx_context_remove_destroying_callback(
	VidgfxContext *context,
	VidgfxContextDestroyingCallback *destroying,
	void *opaque);

//=============================================================================
// D3DTexture C API

#if VIDGFX_D3D_ENABLED

//-----------------------------------------------------------------------------
// Constructor/destructor

API_EXPORT VidgfxD3DTex *vidgfx_tex_get_d3dtex(
	VidgfxTex *tex);
API_EXPORT VidgfxTex *vidgfx_d3dtex_get_tex(
	VidgfxD3DTex *tex);

//-----------------------------------------------------------------------------
// Methods

API_EXPORT HDC vidgfx_d3dtex_get_dc(
	VidgfxD3DTex *tex);
API_EXPORT void vidgfx_d3dtex_release_dc(
	VidgfxD3DTex *tex);

#endif // VIDGFX_D3D_ENABLED

//=============================================================================
// D3DContext C API

#if VIDGFX_D3D_ENABLED

struct ID3D10Device;
struct ID3D10Texture2D;
struct IDXGIFactory1; // DXGI 1.1

//-----------------------------------------------------------------------------
// Static methods

API_EXPORT HRESULT vidgfx_d3d_create_dxgifactory1_dyn(
	IDXGIFactory1 **factory_out);
API_EXPORT void vidgfx_d3d_log_display_adapters();

//-----------------------------------------------------------------------------
// Constructor/destructor

API_EXPORT VidgfxD3DContext *vidgfx_d3dcontext_new();
API_EXPORT void vidgfx_d3dcontext_destroy(
	VidgfxD3DContext *context);

API_EXPORT VidgfxD3DContext *vidgfx_context_get_d3dcontext(
	VidgfxContext *context);
API_EXPORT VidgfxContext *vidgfx_d3dcontext_get_context(
	VidgfxD3DContext *context);

//-----------------------------------------------------------------------------
// Methods

API_EXPORT bool vidgfx_d3dcontext_is_valid(
	VidgfxD3DContext *context);

API_EXPORT bool vidgfx_d3dcontext_init(
	VidgfxD3DContext *context,
	HWND hwnd,
	const QSize &size,
	const QColor &resize_border_col);
API_EXPORT ID3D10Device *vidgfx_d3dcontext_get_device(
	VidgfxD3DContext *context);
API_EXPORT bool vidgfx_d3dcontext_has_dxgi11(
	VidgfxD3DContext *context);
API_EXPORT bool vidgfx_d3dcontext_has_bgra_tex_support(
	VidgfxD3DContext *context);
API_EXPORT VidgfxTex *vidgfx_d3dcontext_new_gdi_tex(
	VidgfxD3DContext *context,
	const QSize &size);
API_EXPORT VidgfxTex *vidgfx_d3dcontext_open_shared_tex(
	VidgfxD3DContext *context,
	HANDLE shared_handle);
API_EXPORT VidgfxTex *vidgfx_d3dcontext_open_dx10_tex(
	VidgfxD3DContext *context,
	ID3D10Texture2D *tex);

//-----------------------------------------------------------------------------
// Signals

typedef void VidgfxD3DContextDxgi11ChangedCallback(
	void *opaque, VidgfxD3DContext *context, bool has_dxgi11);
typedef void VidgfxD3DContextBgraTexSupportChangedCallback(
	void *opaque, VidgfxD3DContext *context, bool has_bgra_tex_support);

API_EXPORT void vidgfx_d3dcontext_add_dxgi11_changed_callback(
	VidgfxD3DContext *context,
	VidgfxD3DContextDxgi11ChangedCallback *dxgi11_changed,
	void *opaque);
API_EXPORT void vidgfx_d3dcontext_remove_dxgi11_changed_callback(
	VidgfxD3DContext *context,
	VidgfxD3DContextDxgi11ChangedCallback *dxgi11_changed,
	void *opaque);
API_EXPORT void vidgfx_d3dcontext_add_bgra_tex_support_changed_callback(
	VidgfxD3DContext *context,
	VidgfxD3DContextBgraTexSupportChangedCallback *bgra_tex_support_changed,
	void *opaque);
API_EXPORT void vidgfx_d3dcontext_remove_bgra_tex_support_changed_callback(
	VidgfxD3DContext *context,
	VidgfxD3DContextBgraTexSupportChangedCallback *bgra_tex_support_changed,
	void *opaque);

#endif // VIDGFX_D3D_ENABLED

#undef API_EXPORT

//#ifdef __cplusplus
//}
//#endif

#endif // LIBVIDGFX_H
