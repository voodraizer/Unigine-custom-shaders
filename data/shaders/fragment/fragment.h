/* Copyright (C) 2005-2020, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2.11.0.1 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */


#ifndef FRAGMENT_SAMPLE_BASE
	#define FRAGMENT_SAMPLE_BASE
	
	#include <core/shaders/common/fragment.h>
	
	#ifdef PARALLAX
		#include <core/shaders/common/parallax.h>
	#endif
	
	// input texture
	INIT_TEXTURE(0, TEX_COLOR)
	INIT_TEXTURE(1, TEX_SHADING)
	INIT_TEXTURE(2, TEX_NORMAL)
	
	#ifdef DETAIL
		INIT_TEXTURE(3, TEX_COLOR_DETAIL)
		INIT_TEXTURE(4, TEX_SHADING_DETAIL)
		INIT_TEXTURE(5, TEX_NORMAL_DETAIL)
		
		#ifdef DETAIL_MASK
			INIT_TEXTURE(6, TEX_DETAIL_MASK)
		#endif
	#endif
	
	#ifdef AO_MAP
		INIT_TEXTURE(7, TEX_AO)
	#endif
	
	#ifdef MICROFIBER_MAP && SPECULAR
		INIT_TEXTURE(9, TEX_MICROFIBER)
	#endif
	
	#ifdef TRANSLUCENT_MAP
		INIT_TEXTURE(11, TEX_TRANSLUCENT)
	#endif
	
	#ifdef PARALLAX
		INIT_TEXTURE(10, TEX_PARALLAX)
	#endif
	
	#ifdef LIGHTMAP
		INIT_TEXTURE(8, TEX_LIGHTMAP)
	#endif
	
	#ifdef BEVEL || CURVATURE
		INIT_TEXTURE(12, TEX_PROCEDURAL_EFFECTS)
	#endif
	
	#ifdef NOISE_2D
		INIT_TEXTURE(13, TEX_NOISE_2D)
	#endif
	
	#ifdef NOISE_3D
		INIT_TEXTURE_3D(14, TEX_NOISE_3D)
		INIT_TEXTURE(15, TEX_NOISE_3D_GRADIENT)
	#endif
	
	#ifdef JITTER_TRANSPARENCY
		INIT_TEXTURE_ARRAY(16, TEX_NOISE)
	#endif
	
	CBUFFER(parameters)
		
		UNIFORM float4	m_color;
		UNIFORM float4	m_shading;
		UNIFORM float	m_normal_scale;
		UNIFORM float	m_translucent;
		
		#ifdef SPECULAR
			UNIFORM float m_microfiber;
		#endif
		
		UNIFORM float m_opacity_fresnel;
		UNIFORM float m_opacity_fresnel_pow;
		
		UNIFORM float m_transparent;
		UNIFORM float m_transparent_pow;
		
		UNIFORM float4 m_uv_transform;
		
		UNIFORM float4 m_ao_uv_transform;
		
		UNIFORM float m_triplanar_blend;

		#ifdef BLENDING
			UNIFORM float m_blend_factor;
			UNIFORM float m_blend_falloff;
		#endif
		
		#ifdef DETAIL
			UNIFORM float4	m_detail_color;
			UNIFORM float4	m_detail_shading;
			
			UNIFORM float4	m_detail_color_visible;
			UNIFORM float4	m_detail_shading_visible;
			UNIFORM float	m_detail_normal_visible;
			
			UNIFORM float	m_detail_visible;
			UNIFORM float	m_detail_visible_threshold;
			
			#ifdef DETAIL_ANGLE_FADE
				UNIFORM float m_detail_angle_fade;
				UNIFORM float m_detail_angle_fade_threshold;
			#endif
			
			#ifdef !DETAIL_MAPPING_BASE
				UNIFORM float4 m_detail_uv_transform;
			#endif
			
			UNIFORM float m_detail_triplanar_blend;
			
			#ifdef DETAIL_MASK
				UNIFORM float4 m_detail_mask_uv_transform;
				UNIFORM float m_detail_mask_triplanar_blend;
			#endif
		#endif
		
		#ifdef EMISSION
			#ifdef !EMISSION_MAPPING_BASE
				UNIFORM float4 m_emission_uv_transform;
			#endif
			UNIFORM float m_emission_triplanar_blend;
		#endif
		
		#ifdef PARALLAX
			PARALLAX_PARAMETERS
			#ifdef PARALLAX_SHADOW
				PARALLAX_SHADOW_PARAMETERS
			#endif
		#endif
		
		#ifdef MIP_BIAS
			UNIFORM float	m_mip_bias;
		#endif
		
		#ifdef EMISSION
			UNIFORM float4	m_emission_color;
			UNIFORM float	m_emission_scale;
			#ifdef CROSS_SECTION
				UNIFORM float4	m_cross_section_color;
			#endif
		#endif
		
		#ifdef AUXILIARY
			UNIFORM float4 m_auxiliary_color;
		#endif
		
		#ifdef LIGHTMAP
			UNIFORM float m_lightmap_scale;
			UNIFORM float m_lightmap_gamma;
			UNIFORM float4 m_lightmap_uv_transform;
		#endif
		
		#ifdef BEVEL
			UNIFORM float	m_bevel_scale;
			UNIFORM float4	m_bevel_uv_transform;
			#ifdef BEVEL_MAPPING_TRIPLANAR
				UNIFORM float m_bevel_blend;
			#endif
		#endif
		
		#ifdef CURVATURE
			UNIFORM float	m_curvature_scale_cavity;
			UNIFORM float	m_curvature_scale_convexity;
			UNIFORM float4	m_curvature_uv_transform;
		#endif
		
		#ifdef NOISE_2D
			UNIFORM float	m_noise_2d_scale;
			UNIFORM float4	m_noise_2d_uv_transform;
		#endif
		
		#ifdef NOISE_3D
			UNIFORM float	m_noise_3d_scale;
			UNIFORM float3	m_noise_3d_transform;
		#endif
		
		#ifdef ANGLE_FADE
			UNIFORM float m_angle_power;
			UNIFORM float m_angle_offset;
		#endif
	END
	
#else
	
	IF_DATA(DATA_OBLIQUE_FRUSTUM)
		if(DATA_OBLIQUE_FRUSTUM > 0.0f) discard;
	ENDIF
	
	IF_DATA(DATA_ALPHA_FADE)
		alphaFadeDiscard(DATA_ALPHA_FADE, IN_POSITION.xy);
	ENDIF
	
	float front_face = 1.0f;
	#ifdef TWO_SIDED
		front_face = IN_FRONTFACE ? 1.0f : -1.0f;
	#endif
	
	float3 OBJECT_NORMAL = DATA_OBJECT_NORMAL;
	float3 WORLD_NORMAL = DATA_WORLD_NORMAL;

	#ifdef TWO_SIDED && BACKSIDE_NORMAL_TO_BACK_SIDE
		OBJECT_NORMAL *= front_face;
		WORLD_NORMAL *= front_face;
	#endif
	
	#ifdef METALNESS
		GBuffer gbuffer = GBufferDefault();
		#define GBUFFER gbuffer
	#else
		GBufferSpecular gbuffer_s = GBufferSpecularDefault();
		#define GBUFFER gbuffer_s
	#endif
	
	#include <core/shaders/mesh/common/uv_select.h>
	
	// TBN matrix
	float3 T = DATA_TANGENT;
	float3 B = float3_one;
	float3 N = DATA_NORMAL;
	
	#ifdef CALCULATE_TANGENT_SPACE
		calculateTBN(T, B, N, DATA_POSITION, base_texcoord);
	#else
		normalizationTBN(T, B, N, DATA_SIGN_BINORMAL);
	#endif
	
	#ifdef TWO_SIDED && BACKSIDE_NORMAL_TO_BACK_SIDE
		T *= front_face;
		B *= front_face;
		N *= front_face;
	#endif
	
	float3x3 TBN = float3x3(T, B, N);
	
	float4 color = m_color;

	#ifdef BLENDING
		// float blend_factor = m_blend_factor;
		// float blend_falloff = m_blend_falloff;
	#endif
	
	// parallax
	#ifdef PARALLAX
		float3 view = -getPositionToViewDirection(DATA_POSITION);
		
		ParallaxIn parallax_in;
		parallax_in.view_ts = normalize(mul(TBN, view));
		parallax_in.uv = base_texcoord;
		parallax_in.uv_aspect_ratio = m_uv_transform.y / m_uv_transform.x;
		
		PARALLAX_INIT(parallax_in)
		
		#ifdef PARALLAX_CUTOUT
			
			float4 cutout_transform = parallaxCutoutUVTransform(m_parallax_cutout_uv_transform, m_uv_transform);
			
			Parallax parallax = parallaxOcclusionMapping(parallax_in, cutout_transform, TEXTURE_OUT(TEX_PARALLAX));
			
			if(parallax.height < -EPSILON) discard;
			
		#else
			Parallax parallax = parallaxOcclusionMapping(parallax_in, TEXTURE_OUT(TEX_PARALLAX));
		#endif
		
		#ifdef PARALLAX_SHADOW
			ParallaxShadowIn parallax_shadow_in;
			
			float3 light_dir = mul3(s_scattering_sun_dir, s_imodelview);
			parallax_shadow_in.light_ts = normalize(mul(TBN, light_dir));
			
			PARALLAX_SHADOW_INIT(parallax_shadow_in)
			
			float shadow = parallaxShadow(parallax_shadow_in, parallax_in, parallax, TEXTURE_OUT(TEX_PARALLAX));
			color *= shadow;
		#endif
		
		float delta_depth = parallaxDeltaDepth(parallax_in, parallax);
		
		#ifdef PARALLAX_DEPTH_CUTOUT
			float4 position = float4(DATA_POSITION, 1.0f);
			
			position.xyz -= view * delta_depth;
			position = getPosition(position);
			OUT_DEPTH = position.z / position.w;
		#endif
		
		base_texcoord -= parallax.uv_offset;
		parallax.uv_offset /= m_uv_transform.xy;
		
		#ifdef DETAIL
			
			#ifdef (BASE_MAPPING_UV1 && DETAIL_MAPPING_UV1) || (BASE_MAPPING_UV0 && DETAIL_MAPPING_UV0)
				detail_texcoord -= parallax.uv_offset * m_detail_uv_transform.xy;
			#endif
			
			#ifdef DETAIL_MASK && ((BASE_MAPPING_UV1 && DETAIL_MASK_MAPPING_UV1) || (BASE_MAPPING_UV0 && DETAIL_MASK_MAPPING_UV0))
				detail_mask_texcoord -= parallax.uv_offset * m_detail_mask_uv_transform.xy;
			#endif
			
		#endif
		
	#endif
	
	//ambient occlusion
	#ifdef AO_MAP
		#ifdef AO_MAPPING_BASE
			GBUFFER.occlusion *= TEXTURE_BASE(TEX_AO).r;
		#else
			
			#ifdef AO_MAPPING_UV0
				float2 ao_texcoord = DATA_UV.xy;
			#else
				float2 ao_texcoord = DATA_UV.zw;
			#endif
			
			#ifdef PARALLAX && ((BASE_MAPPING_UV1 && AO_MAPPING_UV1) || (BASE_MAPPING_UV0 && AO_MAPPING_UV0))
				ao_texcoord -= parallax.uv_offset;
			#endif
			
			ao_texcoord = uvTransform(ao_texcoord, m_ao_uv_transform);
			
			GBUFFER.occlusion *= TEXTURE(TEX_AO, ao_texcoord).r;
		#endif
	#endif
	
	#ifdef VERTEX_COLOR
		#ifdef VERTEX_AO_R
			GBUFFER.occlusion *= DATA_VERTEX_COLOR.r;
		#elif VERTEX_AO_G
			GBUFFER.occlusion *= DATA_VERTEX_COLOR.g;
		#elif VERTEX_AO_B
			GBUFFER.occlusion *= DATA_VERTEX_COLOR.b;
		#elif VERTEX_AO_A
			GBUFFER.occlusion *= DATA_VERTEX_COLOR.a;
		#endif
	#endif
	
	// lightmap
	#ifdef (VERTEX_COLOR && VERTEX_LIGHTMAP) || LIGHTMAP
		
		GBUFFER.lightmap = float3_zero;
		
		#ifdef LIGHTMAP
			
			#ifdef LIGHTMAP_MAPPING_BASE
				GBUFFER.lightmap = TEXTURE_BASE(TEX_LIGHTMAP).rgb;
			#else
				#ifdef LIGHTMAP_MAPPING_UV0
					float2 lightmap_texcoord = DATA_UV.xy;
				#else
					float2 lightmap_texcoord = DATA_UV.zw;
				#endif
				
				#ifdef PARALLAX && ((BASE_MAPPING_UV1 && LIGHTMAP_MAPPING_UV1) || (BASE_MAPPING_UV0 && LIGHTMAP_MAPPING_UV0))
					lightmap_texcoord -= parallax.uv_offset;
				#endif
				
				lightmap_texcoord = uvTransform(lightmap_texcoord, m_lightmap_uv_transform);
				
				GBUFFER.lightmap = TEXTURE(TEX_LIGHTMAP, lightmap_texcoord).rgb;
			#endif
			
			GBUFFER.lightmap = pow(GBUFFER.lightmap * m_lightmap_scale, to_float3(m_lightmap_gamma));
			
		#endif
		
		#ifdef VERTEX_COLOR && VERTEX_LIGHTMAP
			#ifdef LIGHTMAP
				GBUFFER.lightmap += pow(DATA_VERTEX_COLOR.rgb * m_lightmap_scale, to_float3(m_lightmap_gamma));
			#else
				GBUFFER.lightmap += DATA_VERTEX_COLOR.rgb;
			#endif
		#endif
		
	#endif
	
	// bevel or curvature
	#ifdef OUT_GBUFFER_FEATURES
		#ifdef BEVEL
			float4 bevel = TEXTURE_PROCEDURAL_EFFECTS(TEX_PROCEDURAL_EFFECTS);
			GBUFFER.bevel.r = bevel.r;
			GBUFFER.bevel.r *= m_bevel_scale;
		#endif
		#ifdef CURVATURE
			float4 curvature_texcoord = uvTransform(DATA_VERTEX_POSITION.xyyz, m_curvature_uv_transform);
			WEIGHT_TRIPLANAR(curvature_weight, 1.0f)
			float4 curvature = TEXTURE_TRIPLANAR(TEX_PROCEDURAL_EFFECTS, curvature_texcoord, curvature_weight);
			GBUFFER.curvature = float2(curvature.g * m_curvature_scale_cavity, curvature.b * m_curvature_scale_convexity);
		#endif
	#endif
	
	// base shading
	float4 shading		= TEXTURE_BASE(TEX_SHADING) * m_shading;
	
	#ifdef !SPECULAR_MAP && METALNESS
		shading.b = m_shading.b;
	#endif
	
	float4 normal_map	= TEXTURE_BASE_NORMAL(TEX_NORMAL);
	color				*= TEXTURE_BASE(TEX_COLOR);
	
	// normal map
	#ifdef TRIPLANAR_NORMAL
		#ifdef BASE_MAPPING_TRIPLANAR //obj to world
			float3 world_normal = normal_map.xyz;
			world_normal = normalize(mul(DATA_TRANSFORM, world_normal));
		#elif BASE_MAPPING_TRIPLANAR_WORLD
			float3 world_normal = normal_map.xyz;
			world_normal = normalize(mul3(s_modelview, world_normal));
		#else//ts to world
			float3 world_normal = normal_map.xyz;
			world_normal.z = getNormalZ(world_normal);
			world_normal = normalize(mul(world_normal, TBN));
		#endif
		
	#else
		#ifdef NORMAL_MAP_OBJECT_SPACE //obj to ts
			float3 ts_normal = normal_map.xzy;
			ts_normal.y *= -1.0f;
			ts_normal = mul(DATA_TRANSFORM, ts_normal);
			ts_normal = mul(TBN, normalize(ts_normal)) / max(m_normal_scale, EPSILON);
		#else //ts
			float3 ts_normal = normal_map.xyz;
			ts_normal.z = getNormalZ(ts_normal);
		#endif
	#endif
	
	#ifdef NOISE_2D
		float3 noise_2d = TEXTURE(TEX_NOISE_2D, uvTransform(DATA_NOISE_UV.xy, m_noise_2d_uv_transform)).rgb;
		color.rgb = saturate(color.rgb + (noise_2d * 2.0f - 1.0f) * m_noise_2d_scale);
	#endif
	
	#ifdef NOISE_3D
		float noise_texcoord = TEXTURE(TEX_NOISE_3D, DATA_NOISE_UV * m_noise_3d_transform.xyz).x;
		float3 noise_3d = TEXTURE(TEX_NOISE_3D_GRADIENT, float2(noise_texcoord, 0.0f)).rgb;
		color.rgb = saturate(color.rgb + (noise_3d * 2.0f - 1.0f) * m_noise_3d_scale);
	#endif
	
	#ifdef VERTEX_COLOR
		#ifdef METALNESS
			#ifdef VERTEX_ALBEDO
				color.rgb *= DATA_VERTEX_COLOR.rgb;
			#endif
		#else
			#ifdef VERTEX_DIFFUSE
				color.rgb *= DATA_VERTEX_COLOR.rgb;
			#endif
			
			#ifdef VERTEX_SPECULAR
				shading.rgb *= DATA_VERTEX_COLOR.rgb;
			#endif
		#endif
	#endif
	
	// detail shading
	#ifdef DETAIL
		float4 detail_shading = m_detail_shading;
		
		#ifdef SPECULAR_MAP
			detail_shading.xyz *= TEXTURE_DETAIL(TEX_SHADING_DETAIL).xyz;
		#else
			detail_shading.xy *= TEXTURE_DETAIL(TEX_SHADING_DETAIL).xy;
		#endif
		
		float4 detail_color = TEXTURE_DETAIL(TEX_COLOR_DETAIL) * m_detail_color;
		
		#ifdef DETAIL_MASK
			detail_color.a *= TEXTURE_DETAIL_MASK(TEX_DETAIL_MASK).r;
		#endif
		
		#ifdef VERTEX_COLOR
			#ifdef VERTEX_DETAIL_MASK_R
				detail_color.a *= DATA_VERTEX_COLOR.r;
			#elif VERTEX_DETAIL_MASK_G
				detail_color.a *= DATA_VERTEX_COLOR.g;
			#elif VERTEX_DETAIL_MASK_B
				detail_color.a *= DATA_VERTEX_COLOR.b;
			#elif VERTEX_DETAIL_MASK_A
				detail_color.a *= DATA_VERTEX_COLOR.a;
			#endif
		#endif
		
		#ifdef DETAIL_ANGLE_FADE
			detail_color.a *= saturate((DATA_DETAIL_ANGLE_FADE - m_detail_angle_fade + m_detail_angle_fade_threshold * 0.5f) / max(m_detail_angle_fade_threshold, EPSILON));
		#endif
		
		detail_color.a = clamp((detail_color.a - (1.0f - m_detail_visible * 0.5f) + m_detail_visible_threshold * 0.5f) / max(m_detail_visible_threshold, EPSILON), EPSILON, 1.0f);
		
		#ifdef DETAIL_BLEND_OVERLAY
			#define DETAIL_BLEND(V0, V1, K) V0 = overlay(V0, V1, K * detail_color.a);
		#elif DETAIL_BLEND_MULTIPLY
			#define DETAIL_BLEND(V0, V1, K) V0 *= lerp(V1, float4_one, float4_one - K * detail_color.a);
		#else
			#define DETAIL_BLEND(V0, V1, K) V0 = lerp(V0, V1, K * detail_color.a);
		#endif
		
		DETAIL_BLEND(color,		detail_color,	to_float4(m_detail_color_visible.r))
		DETAIL_BLEND(shading,	detail_shading,	m_detail_shading_visible)
	#endif	
	
	// normal
	#ifdef TRIPLANAR_NORMAL // world + world
		#ifdef DETAIL
			float3 world_detail_normal = TEXTURE_DETAIL_NORMAL(TEX_NORMAL_DETAIL).xyz;
			
			#ifndef NORMAL_SUBSTITUTE
				
				#ifdef DETAIL_MAPPING_TRIPLANAR // obj to world
					world_detail_normal = lerp(OBJECT_NORMAL, world_detail_normal, m_detail_normal_visible * detail_color.a);
					world_detail_normal = normalize(mul(DATA_TRANSFORM, world_detail_normal));
				#elif DETAIL_MAPPING_TRIPLANAR_WORLD
					world_detail_normal = lerp(WORLD_NORMAL, world_detail_normal, m_detail_normal_visible * detail_color.a);
					world_detail_normal = normalize(mul3(s_modelview, world_detail_normal));
				#else // ts to world
					world_detail_normal.xy *= m_detail_normal_visible * detail_color.a;
					world_detail_normal.z = getNormalZ(world_detail_normal.xy);
					world_detail_normal = normalize(mul(world_detail_normal, TBN));
				#endif
				
				world_normal = reorientNormalBlend(world_detail_normal, world_normal, N);
			#else
				
				#ifdef DETAIL_MAPPING_TRIPLANAR // obj to world
					world_normal = lerp(world_normal, normalize(mul(DATA_TRANSFORM, world_detail_normal)), m_detail_normal_visible * detail_color.a);
				#elif DETAIL_MAPPING_TRIPLANAR_WORLD
					world_normal = lerp(world_normal, normalize(mul3(s_modelview, world_detail_normal)), m_detail_normal_visible * detail_color.a);
				#else // ts to world
					world_detail_normal.z = getNormalZ(world_detail_normal.xy);
					world_normal = lerp(world_normal, normalize(mul(world_detail_normal, TBN)), m_detail_normal_visible * detail_color.a);
				#endif
				
			#endif
			
		#endif
		
		world_normal = lerp(N, world_normal, m_normal_scale);
		GBUFFER.normal = normalize(world_normal);
	#else
		#ifdef DETAIL //tan + tan
			float3 ts_detail_normal = TEXTURE_DETAIL_NORMAL(TEX_NORMAL_DETAIL).xyz;
			
			#ifndef NORMAL_SUBSTITUTE
				ts_detail_normal.xy *= m_detail_normal_visible * detail_color.a;
				ts_detail_normal.z = getNormalZ(ts_detail_normal.xy);
				ts_normal = reorientNormalBlend(ts_detail_normal, ts_normal);
			#else
				ts_detail_normal.z = getNormalZ(ts_detail_normal.xy);
				ts_normal = lerp(ts_normal, ts_detail_normal, m_detail_normal_visible * detail_color.a);
			#endif
		#endif
		
		ts_normal = lerp(float3(0.0, 0.0, 1.0), ts_normal, m_normal_scale);
		GBUFFER.normal = normalize(mul(normalize(ts_normal), TBN));
	#endif
	
	// transparent
	#ifdef TRANSPARENT || ALPHA_TEST
		#ifdef OPACITY_MAP_NORMAL
			#ifdef NORMAL_MAP_OBJECT_SPACE
				color.a = normal_map.a * m_color.a;
			#else
				color.a = normal_map.b * m_color.a;
			#endif
		#endif
		
		color.a = pow(color.a * m_transparent, m_transparent_pow);
		
		float fresnel = pow((1.0f - dotFixed(GBUFFER.normal, getPositionToViewDirection(DATA_POSITION))), m_opacity_fresnel_pow);
		fresnel = lerp(fresnel, 1.0f, color.a);
		
		color.a = lerp(color.a, fresnel, m_opacity_fresnel);
		
		GBUFFER.transparent = color.a;
		
	#endif
	
	#ifdef METALNESS
		GBUFFER.albedo		= color.rgb;
		GBUFFER.metalness	= shading.r;
		GBUFFER.roughness	= shading.g;
		GBUFFER.f0 = shading.b;
		
		#ifdef MICROFIBER_MAP
			GBUFFER.microfiber = shading.a;
		#else
			GBUFFER.microfiber = m_shading.a;
		#endif
		
	#else
		GBUFFER.diffuse		= color.rgb;
		GBUFFER.specular	= shading.rgb;
		GBUFFER.gloss		= shading.a;
		GBUFFER.microfiber	= m_microfiber;
		
		#ifdef MICROFIBER_MAP
			GBUFFER.microfiber *= TEXTURE_BASE(TEX_MICROFIBER).r;
		#endif
		
	#endif
	
	GBUFFER.translucent = m_translucent;
	#ifdef TRANSLUCENT_MAP
		GBUFFER.translucent *= TEXTURE_BASE(TEX_TRANSLUCENT).r;
	#endif
	
	#ifdef !METALNESS
		#undef GBUFFER
		GBuffer gbuffer = specularToMetalness(gbuffer_s);
	#endif
	
	#ifdef BAKING_LIGHTING && (!TWO_SIDED)
		if(!IN_FRONTFACE) {
			gbuffer.albedo = float3_zero;
			gbuffer.metalness = 0.0f;
			gbuffer.roughness = 1.0f;
			gbuffer.f0 = 0.0f;
			gbuffer.microfiber = 0.0f;
		}
	#endif
	
	#ifdef CROSS_SECTION && TWO_SIDED && EMISSION
		if(!IN_FRONTFACE) {
			gbuffer.albedo = float3_zero;
			gbuffer.metalness = 0.0f;
			gbuffer.roughness = 1.0f;
			gbuffer.f0 = 0.0f;
			gbuffer.microfiber = 0.0f;
		}
	#endif
	
#endif
