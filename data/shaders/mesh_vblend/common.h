#include <core/shaders/mesh/common/common.h>
#ifdef VERTEX
	#include <core/shaders/mesh/common/vertex.h>
#elif FRAGMENT
	#include <core/shaders/mesh/common/fragment.h>
#endif


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
	
	#ifdef VERTEX_COLOR || VERTEX_COLOR_BLENDING || VERTEX_COLOR_BLENDING_G
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


#ifdef VERTEX

    STRUCT(VERTEX_OUT)
        INIT_POSITION					// Out projected position
        INIT_OUT(float4, 0)				// Texcoord (uv)
        INIT_OUT(float4, 1)				// Vert color
    END

#endif