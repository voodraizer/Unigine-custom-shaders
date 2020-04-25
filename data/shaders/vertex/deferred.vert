// Include Unified Unigine Shader Language (UUSL) header
#include <core/shaders/common/common.h>

// Input data struct
STRUCT(VERTEX_IN)
	INIT_ATTRIBUTE(float4,0,POSITION)	// Vertex position
	INIT_ATTRIBUTE(float4,1,TEXCOORD0)	// Vertex texcoord (uv)
	INIT_ATTRIBUTE(float4,2,TEXCOORD1)	// Vertex basis tangent
	INIT_ATTRIBUTE(float4,3,TEXCOORD2)	// Vertex color
END

// Our output vertex data struct
STRUCT(VERTEX_OUT)
	INIT_POSITION					// Out projected position
	INIT_OUT(float4,0)				// Texcoord (uv)
END

CBUFFER(parameters)
	UNIFORM float4	m_detail_albedo_color;
END

MAIN_BEGIN(VERTEX_OUT,VERTEX_IN)
	
	// Get transform with scale and rotation (without translation)
	float4 row_0 = s_transform[0];
	float4 row_1 = s_transform[1];
	float4 row_2 = s_transform[2];
	
	// Get Modelview-space transform
	float4 in_vertex = float4(IN_ATTRIBUTE(0).xyz,1.0f);
	float4 position = mul4(row_0,row_1,row_2,in_vertex);
	
	// float4 m_detail_albedo_color = float4(1, 0, 0, 1);
	// m_detail_albedo_color = float4(1, 0, 0, 1);

	// Set output UV
	float4 texcoord = IN_ATTRIBUTE(1);
	OUT_DATA(0) = texcoord;
	
	// Set output position
	OUT_POSITION = getPosition(position);
MAIN_END

// end