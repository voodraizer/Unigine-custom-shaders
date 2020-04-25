// Include the UUSL language header
#include <core/shaders/common/fragment.h>

// Adds a texture sampler
INIT_TEXTURE(0, TEX_COLOR)

STRUCT(FRAGMENT_IN)
	INIT_POSITION				// Projected position
	INIT_IN(float4,0)			// Texcoord (uv)
	INIT_IN(float3,1)			// Vertex TBN (X)
	INIT_IN(float3,2)			// Vertex TBN (Y)
	INIT_IN(float3,3)			// Vertex TBN (Z)
END

CBUFFER(parameters)
	UNIFORM float4	m_detail_albedo_color;
END

MAIN_BEGIN_DEFERRED(FRAGMENT_IN)
	
	// Get the UV coords
	float4 texcoord = IN_DATA(0);

	// Get the texture data
	float4 texture_data = TEXTURE(TEX_COLOR,texcoord.xy);
	
	// Define the normal of a fragment in tangent-space
	STATICVAR float3 tangentspace_normal = float3(0.0f,0.0f,1.0f);
	
	// Calculate the view-space normal
	float3 viewspace_normal;
	viewspace_normal.x = dot(IN_DATA(1),tangentspace_normal);
	viewspace_normal.y = dot(IN_DATA(2),tangentspace_normal);
	viewspace_normal.z = dot(IN_DATA(3),tangentspace_normal);
	viewspace_normal = normalize(viewspace_normal);
	
	// Fill G-Buffer: set the calculated normal and albedo color of the texture
	GBuffer gbuffer = GBufferDefault();

	float3 color = m_detail_albedo_color.rgb;

	gbuffer.albedo = color; //texture_data.rgb;
	gbuffer.normal = viewspace_normal;

	setGBuffer(gbuffer);
	
MAIN_END

// end