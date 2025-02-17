#pragma once

#include "qcommon/types.h"
#include "client/renderer/backend.h"
#include "client/renderer/material.h"
#include "client/renderer/model.h"
#include "client/renderer/shader.h"
#include "client/renderer/srgb.h"
#include "cgame/ref.h"

enum ShadowQuality {
	ShadowQuality_Low,
	ShadowQuality_Medium,
	ShadowQuality_High,
	ShadowQuality_Ultra,
};

struct ShadowParameters {
	u32 num_cascades;
	float cascade_dists[ 4 ];
	u32 shadowmap_res;
	u32 entity_cascades;
};

/*
 * stuff that gets set once at the beginning of the frame but is needed all over the place
 */
struct FrameStatic {
	u32 viewport_width, viewport_height;
	u32 last_viewport_width, last_viewport_height;
	Vec2 viewport;
	float aspect_ratio;
	int msaa_samples;
	ShadowQuality shadow_quality;
	ShadowParameters shadow_parameters;

	UniformBlock view_uniforms;
	UniformBlock ortho_view_uniforms;
	UniformBlock shadowmap_view_uniforms[ 4 ];
	UniformBlock shadow_uniforms;
	UniformBlock identity_model_uniforms;
	UniformBlock identity_material_uniforms;
	UniformBlock fog_uniforms;
	UniformBlock blue_noise_uniforms;

	Mat4 V, inverse_V;
	Mat4 P, inverse_P;
	Vec3 light_direction;
	Vec3 position;
	float vertical_fov;
	float near_plane;

	Framebuffer silhouette_gbuffer;
	Framebuffer msaa_fb;
	Framebuffer postprocess_fb;
	Framebuffer msaa_fb_onlycolor;
	Framebuffer postprocess_fb_onlycolor;
	Framebuffer shadowmap_fb[ 4 ];

	TextureArray shadowmap_texture_array;

	u8 particle_update_pass;
	u8 shadowmap_pass[ 4 ];
	u8 world_opaque_prepass_pass;
	u8 world_opaque_pass;
	u8 add_world_outlines_pass;

	u8 write_silhouette_gbuffer_pass;

	u8 nonworld_opaque_pass;
	u8 sky_pass;
	u8 transparent_pass;

	u8 add_silhouettes_pass;

	u8 ui_pass;

	u8 postprocess_pass;

	u8 post_ui_pass;
};

struct DynamicMesh {
	const Vec3 * positions;
	const Vec2 * uvs;
	const RGBA8 * colors;
	const u16 * indices;

	u32 num_vertices;
	u32 num_indices;
};

extern FrameStatic frame_static;

void InitRenderer();
void ShutdownRenderer();

void RendererBeginFrame( u32 viewport_width, u32 viewport_height );
void RendererSetView( Vec3 position, EulerDegrees3 angles, float vertical_fov );
void RendererSubmitFrame();

const Texture * BlueNoiseTexture();
void DrawFullscreenMesh( const PipelineState & pipeline );

PipelineState MaterialToPipelineState( const Material * material, Vec4 color = vec4_white, bool skinned = false );

void Draw2DBox( float x, float y, float w, float h, const Material * material, Vec4 color = vec4_white );
void Draw2DBoxUV( float x, float y, float w, float h, Vec2 topleft_uv, Vec2 bottomright_uv, const Material * material, Vec4 color );
// void DrawRotatedBox( float x, float y, float w, float h, float angle, const Material * material, RGBA8 color );

u16 DynamicMeshBaseIndex();
void DrawDynamicMesh( const PipelineState & pipeline, const DynamicMesh & mesh );

UniformBlock UploadModelUniforms( const Mat4 & M );
UniformBlock UploadMaterialUniforms( const Vec4 & color, const Vec2 & texture_size, float specular, float shininess, Vec3 tcmod_row0 = Vec3( 1, 0, 0 ), Vec3 tcmod_row1 = Vec3( 0, 1, 0 ) );

const char * ShadowQualityToString( ShadowQuality mode );
