#include <shaders/mesh_vblend/common.h>



MAIN_BEGIN_VERTEX(FRAGMENT_IN)
	#include <core/shaders/mesh/common/vertex.h>

	#ifndef VERTEX_COLOR
		#ifdef VERTEX_COLOR_BLENDING
			IF_DATA(DATA_VERTEX_COLOR)
				DATA_VERTEX_COLOR = ATTRIBUTE_COLOR;
			ENDIF
		#endif
	#endif
	
MAIN_END


// end