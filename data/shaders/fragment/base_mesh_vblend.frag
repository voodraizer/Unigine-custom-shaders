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

#ifdef BLENDING
	INIT_TEXTURE(3, TEX_ALBEDO_BLEND)
	INIT_TEXTURE(4, TEX_BLEND_SHADING)
	INIT_TEXTURE(5, TEX_BLEND_NORMAL)
	INIT_TEXTURE(6, TEX_BLEND_MASK)
#endif

// CBUFFER(parameters)
	// UNIFORM float m_blend_factor;
	// UNIFORM float m_blend_falloff;
	// UNIFORM float m_blend_alpha;

	// UNIFORM float m_dirt_alpha;
	// UNIFORM float m_dirt_gloss_coeff;
// END

UNIFORM float m_blend_factor;
UNIFORM float m_blend_falloff;
UNIFORM float m_blend_alpha;

UNIFORM float m_dirt_alpha;
UNIFORM float m_dirt_roughness_coeff;

MAIN_BEGIN_DEFERRED(FRAGMENT_IN)
	
	#include <core/shaders/mesh/common/fragment.h>
	// #include <shaders/fragment/fragment.h>
	
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
	// #endif

	#ifdef BLENDING
	
		float4 blend_color =  TEXTURE_BASE(TEX_ALBEDO_BLEND);
		float4 blend_shading = TEXTURE_BASE(TEX_BLEND_SHADING);
		float4 blend_normal = TEXTURE_BASE_NORMAL(TEX_BLEND_NORMAL);
		float4 blend_mask =  TEXTURE_BASE(TEX_BLEND_MASK);

		float3 final_albedo;
		float3 final_shading = float3(0, 0, 0);
		float3 final_normal = float3(0, 0, 1.0);

		// Blend 1-st texture (R-channel).
		// m_blend_factor = 0.5f; m_blend_falloff = 4.0f; m_blend_alpha = 1.3f;

		float blend_coeff = m_blend_alpha * blend_mask.r * (1 + m_blend_factor) * DATA_VERTEX_COLOR.r;
		blend_coeff = saturate(pow(blend_coeff, m_blend_falloff));
		final_albedo = lerp(blend_color.rgb, color.rgb, blend_coeff);

		final_shading.g = lerp(blend_shading.g, shading.g, blend_coeff);

		float3 ts_blend_normal = blend_normal.xyz;
		ts_blend_normal.z = getNormalZ(ts_blend_normal);
		ts_blend_normal = lerp(float3(0.0, 0.0, 1.0), ts_blend_normal, m_normal_scale);
		final_normal = lerp(normalize(mul(normalize(ts_blend_normal), TBN)), normalize(mul(normalize(ts_normal), TBN)), blend_coeff);

		// Dirt (B-channel).
		blend_coeff = (1 - blend_mask.b * m_dirt_alpha * DATA_VERTEX_COLOR.b);
		final_albedo *= blend_coeff;

		final_shading.g = final_shading.g * lerp(final_shading.g * m_dirt_roughness_coeff, final_shading.g, blend_coeff);



		gbuffer.albedo = final_albedo;
		gbuffer.metalness = final_shading.r;
		gbuffer.roughness = final_shading.g;
		gbuffer.normal = final_normal;

		// DEBUG.
		// gbuffer.albedo = lerp(float3(0, 0, 0), float3(1, 0, 0), DATA_VERTEX_COLOR.r);
		// gbuffer.albedo = DATA_VERTEX_COLOR.rgb;

	#endif
	
	setGBuffer(gbuffer);
	
MAIN_END


// end