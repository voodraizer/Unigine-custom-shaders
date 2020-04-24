

// ================================================================================

// #include <core/shaders/mesh/common/common.h>
// #include <core/shaders/mesh/common/fragment.h>


// STRUCT(FRAGMENT_IN)
// 	INIT_POSITION
// 	INIT_DATA(float4, 0, DATA_UV)	
// 	INIT_DATA(float4, 11, DATA_VERTEX_COLOR)
// END


// MAIN_BEGIN_DEFERRED(FRAGMENT_IN)

// 	GBuffer gbuffer = GBufferDefault();
// 	// gbuffer.albedo = DATA_UV.rgb;
// 	gbuffer.albedo = DATA_VERTEX_COLOR.rgb;//float3(0, 1, 0);
// 	gbuffer.metalness = 0.0f;
// 	gbuffer.roughness = 1.0f;
// 	gbuffer.f0 = 0.0f;
// 	gbuffer.microfiber = 0.0f;
// 	gbuffer.transparent = 0.5f;

// 	setGBuffer(gbuffer);

	
// MAIN_END

// ================================================================================

// #include <core/shaders/mesh/common/common.h>
// #include <core/shaders/mesh/common/fragment.h>

// MAIN_BEGIN_DEFERRED(FRAGMENT_IN)

// 	GBuffer gbuffer = GBufferDefault();
// 	setGBuffer(gbuffer);

// MAIN_END


// #include <core/shaders/mesh/common/common.h>

// #ifdef PARALLAX && PARALLAX_DEPTH_CUTOUT
// 	#define USE_CUSTOM_DEPTH
// #endif

// #define VERTEX_COLOR

#include <core/shaders/mesh/common/fragment.h>


STRUCT(FRAGMENT_IN)
	INIT_POSITION
	
	INIT_DATA(float4, 0, DATA_UV)
	
	INIT_DATA(float3, 1, DATA_NORMAL)
	INIT_DATA(float3, 2, DATA_TANGENT)
	INIT_DATA(float, 3, DATA_SIGN_BINORMAL)
	
	#ifdef OUT_GBUFFER_VELOCITY
		INIT_DATA(float3, 4, DATA_NEW_POSITION)
		INIT_DATA(float3, 5, DATA_OLD_POSITION)
	#endif
	
	#ifdef NOISE_2D || NOISE_3D
		INIT_DATA(float3, 6, DATA_NOISE_UV)
	#endif
	
	INIT_DATA(float3, 7, DATA_WORLD_POSITION)
	INIT_DATA(float3, 8, DATA_VERTEX_POSITION)
	INIT_DATA(float3, 9, DATA_OBJECT_NORMAL)
	INIT_DATA(float3, 10, DATA_WORLD_NORMAL)
	
	#ifdef VERTEX_COLOR
		INIT_DATA(float4, 11, DATA_VERTEX_COLOR)
	#endif
	
	#ifdef DETAIL_ANGLE_FADE0 || DETAIL_ANGLE_FADE1
		INIT_DATA(float, 12, DATA_DETAIL_ANGLE_FADE)
	#endif
	
	INIT_DATA(float3, 13, DATA_POSITION)
	
	INIT_DATA(float, 14, DATA_OBLIQUE_FRUSTUM)
	
	#ifdef ALPHA_FADE
		INIT_DATA(float, 15, DATA_ALPHA_FADE)
	#endif
	
	#ifdef USE_DATA_TRANSFORM
		INIT_DATA(float3x3, 16, DATA_TRANSFORM)
	#endif
	
	#ifdef TWO_SIDED || BAKING_LIGHTING
		INIT_FRONTFACE
	#endif
	
END

#ifdef BLEND_BY_VERTEX_COLOR
	INIT_TEXTURE(3, TEX_ALBEDO_BLEND)
	INIT_TEXTURE(4, TEX_BLEND_SHADING)
	INIT_TEXTURE(5, TEX_BLEND_NORMAL)
	INIT_TEXTURE(6, TEX_BLEND_MASK)

	// #define VERTEX_COLOR
#endif


MAIN_BEGIN_DEFERRED(FRAGMENT_IN)
	
	#include <core/shaders/mesh/common/fragment.h>
	
	// #ifdef OUT_GBUFFER_VELOCITY
	// 	#ifdef PARALLAX
	// 		view *= delta_depth;
			
	// 		float3 new_position = DATA_NEW_POSITION;
	// 		float3 old_position = DATA_OLD_POSITION;
			
	// 		new_position-= mul3(s_old_imodelview, view);
	// 		old_position-= mul3(s_imodelview, view);
			
	// 		new_position = mul4(s_modelview_projection_x, s_modelview_projection_y, s_modelview_projection_w, new_position);
	// 		old_position = mul4(s_modelview_projection_old_x, s_modelview_projection_old_y, s_modelview_projection_old_w, old_position);
			
	// 		gbuffer.velocity = getScreenVelocity(old_position, new_position);
	// 	#else
	// 		gbuffer.velocity = getScreenVelocity(DATA_OLD_POSITION, DATA_NEW_POSITION);
	// 	#endif

	// gbuffer.albedo *= DATA_VERTEX_COLOR.rgb;

	#ifdef BLEND_BY_VERTEX_COLOR
	float4 blend_color =  TEXTURE_BASE(TEX_ALBEDO_BLEND);
	float4 blend_shading = TEXTURE_BASE(TEX_BLEND_SHADING);
	float4 blend_normal = TEXTURE_BASE_NORMAL(TEX_BLEND_NORMAL);
	float4 blend_mask =  TEXTURE_BASE(TEX_BLEND_MASK);

	gbuffer.albedo = lerp(blend_color.rgb, color.rgb, blend_mask.r * DATA_VERTEX_COLOR.r);
	#endif
	
	
	setGBuffer(gbuffer);
	
MAIN_END


// ================================================================================




// #endif