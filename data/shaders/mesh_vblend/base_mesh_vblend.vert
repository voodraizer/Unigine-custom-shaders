#include <shaders/mesh_vblend/common.h>



MAIN_BEGIN_VERTEX(FRAGMENT_IN)
	#include <core/shaders/mesh/common/vertex.h>

	IF_DATA(DATA_VERTEX_COLOR)
		DATA_VERTEX_COLOR = ATTRIBUTE_COLOR;
	ENDIF
	
MAIN_END


// end