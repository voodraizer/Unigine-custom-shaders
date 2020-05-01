#include <shaders/mesh_vblend/common.h>


#ifdef VERTEX_COLOR_BLEND_R
	INIT_TEXTURE(18, TEX_BLEND_ALBEDO_R)
	INIT_TEXTURE(19, TEX_BLEND_SHADING_R)
	INIT_TEXTURE(20, TEX_BLEND_NORMAL_R)
#endif

#ifdef VERTEX_COLOR_BLEND_G
	INIT_TEXTURE(21, TEX_BLEND_ALBEDO_G)
	INIT_TEXTURE(22, TEX_BLEND_SHADING_G)
	INIT_TEXTURE(23, TEX_BLEND_NORMAL_G)
#endif

#ifdef VERTEX_COLOR_BLEND_R || VERTEX_COLOR_BLEND_G
	INIT_TEXTURE(6, TEX_BLEND_MASK)
#endif


CBUFFER(parameters)
	UNIFORM float m_blend_scale_uv_2;
	UNIFORM float m_blend_scale_uv_3;
	UNIFORM float m_blend_scale_uv_4;

	UNIFORM float m_blend_factor_r;
	UNIFORM float m_blend_falloff_r;
	UNIFORM float m_blend_alpha_r;
	UNIFORM float m_blend_normals_r;

	UNIFORM float m_blend_factor_g;
	UNIFORM float m_blend_falloff_g;
	UNIFORM float m_blend_alpha_g;
	UNIFORM float m_blend_normals_g;

	UNIFORM float m_dirt_alpha;
	UNIFORM float m_dirt_roughness_coeff;
END


MAIN_BEGIN_DEFERRED(FRAGMENT_IN)
	#include <core/shaders/mesh/common/fragment.h>
	
	#ifdef OUT_GBUFFER_VELOCITY
		#ifdef PARALLAX
			view *= delta_depth;
			
			float3 new_position = DATA_NEW_POSITION;
			float3 old_position = DATA_OLD_POSITION;
			
			new_position-= mul3(s_old_imodelview, view);
			old_position-= mul3(s_imodelview, view);
			
			new_position = mul4(s_modelview_projection_x, s_modelview_projection_y, s_modelview_projection_w, new_position);
			old_position = mul4(s_modelview_projection_old_x, s_modelview_projection_old_y, s_modelview_projection_old_w, old_position);
			
			gbuffer.velocity = getScreenVelocity(old_position, new_position);
		#else
			gbuffer.velocity = getScreenVelocity(DATA_OLD_POSITION, DATA_NEW_POSITION);
		#endif
	#endif

	// Variables.
	float2 texcoord_orig = DATA_UV.xy;
	float2 texcoord_modif = texcoord_orig;

	float4 blend_color_r = color;
	float4 blend_shading_r = shading;
	float4 blend_normal_r = normal_map;

	float4 blend_color_g = float4(1, 1, 1, 1);
	float4 blend_shading_g = float4(0, 0, 0, 1);
	float4 blend_normal_g = float4(0, 0, 1, 1);

	float blend_coeff = 0;
	float4 blend_mask = float4(1, 1, 1, 1);
	float3 ts_blend_normal = float3(0, 0, 1.0);

	float3 final_albedo = color.rgb;
	float3 final_shading = shading.rgb;
	float3 final_normal = blend_normal_r.xyz;

	final_normal.z = getNormalZ(final_normal);
	final_normal = lerp(float3(0.0, 0.0, 1.0), final_normal, m_normal_scale);
	final_normal = normalize(mul(normalize(ts_normal), TBN));


	#ifdef VERTEX_COLOR_BLEND_R || VERTEX_COLOR_BLEND_G

		texcoord_modif = uvTransform(texcoord_orig, m_blend_scale_uv_3);
		blend_mask =  TEXTURE(TEX_BLEND_MASK, texcoord_modif);

		blend_coeff = m_blend_alpha_r * blend_mask.r * (1 + m_blend_factor_r) * DATA_VERTEX_COLOR.r;
		blend_coeff = saturate(pow(blend_coeff, m_blend_falloff_r));

	#endif

	#ifdef VERTEX_COLOR_BLEND_R

		// Blend 1-st texture (R-channel).
		texcoord_modif = uvTransform(texcoord_orig, m_blend_scale_uv_2);
		blend_color_r = TEXTURE(TEX_BLEND_ALBEDO_R, texcoord_modif);
		blend_shading_r = TEXTURE(TEX_BLEND_SHADING_R, texcoord_modif);
		blend_normal_r = TEXTURE(TEX_BLEND_NORMAL_R, texcoord_modif);

		final_albedo = lerp(blend_color_r.rgb, color.rgb, blend_coeff);
		final_shading.g = lerp(blend_shading_r.g, shading.g, blend_coeff);

		ts_blend_normal = blend_normal_r.xyz;
		ts_blend_normal.z = getNormalZ(ts_blend_normal);
		ts_blend_normal = lerp(float3(0.0, 0.0, 1.0), ts_blend_normal, m_normal_scale);
		final_normal = lerp(normalize(mul(normalize(ts_blend_normal), TBN)), final_normal, blend_coeff * m_blend_normals_r);

		#ifdef PARALLAX

		#endif

	#endif

	#ifdef VERTEX_COLOR_BLEND_G

		// Blend 2-nd texture (G-channel).
		texcoord_modif = uvTransform(texcoord_orig, m_blend_scale_uv_4);

		blend_color_g =  TEXTURE(TEX_BLEND_ALBEDO_G, texcoord_modif);
		blend_shading_g = TEXTURE(TEX_BLEND_SHADING_G, texcoord_modif);
		blend_normal_g = TEXTURE(TEX_BLEND_NORMAL_G, texcoord_modif);

		float blend_coeff_g = m_blend_alpha_g * blend_mask.g * (1 + m_blend_factor_g) * DATA_VERTEX_COLOR.g;
		blend_coeff_g = saturate(pow(blend_coeff_g, m_blend_falloff_g));

		final_albedo = lerp(final_albedo.rgb, blend_color_g.rgb, blend_coeff_g);

		final_shading.g = lerp(final_shading.g, blend_shading_g.g, blend_coeff_g);

		ts_blend_normal = blend_normal_g.xyz;
		ts_blend_normal.z = getNormalZ(ts_blend_normal);
		ts_blend_normal = lerp(float3(0.0, 0.0, 1.0), ts_blend_normal, m_normal_scale);
		final_normal = lerp(final_normal, normalize(mul(normalize(ts_blend_normal), TBN)), blend_coeff_g * m_blend_normals_g);

	#endif

	#ifdef VERTEX_COLOR_BLEND_R || VERTEX_COLOR_BLEND_G
		
		// Dirt (B-channel).
		blend_coeff = (1 - blend_mask.b * m_dirt_alpha * DATA_VERTEX_COLOR.b);
		final_albedo *= blend_coeff;

		final_shading.g = final_shading.g * lerp(final_shading.g * m_dirt_roughness_coeff, final_shading.g, blend_coeff);

	#endif

	// Store.
	gbuffer.albedo = final_albedo;
	gbuffer.metalness = final_shading.r;
	gbuffer.roughness = final_shading.g;
	gbuffer.normal = final_normal;

	setGBuffer(gbuffer);
	
MAIN_END


// end