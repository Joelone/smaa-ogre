

#define VERTEX_SHADER 1

#define EDGE_VS 1

uniform vec4 viewportSize;

#define SMAA_PIXEL_SIZE vec2(viewportSize.z, viewportSize.w)

#define SMAA_PRESET_ULTRA 1

#define SMAA_GLSL_4 1


#ifdef VERTEX_SHADER
#define SMAA_ONLY_COMPILE_VS 1
#else
#define SMAA_ONLY_COMPILE_PS 1
#endif




#include "SMAA.h"








#ifdef EDGE_VS
out vec2 texcoord;
out vec4 offset[3];
out vec4 dummy2;
void main()
{
	texcoord = gl_MultiTexCoord0.xy;
	vec4 dummy1 = vec4(0);
	SMAAEdgeDetectionVS(dummy1, dummy2, texcoord, offset);
	gl_Position = ftransform();
}

#endif


#ifdef EDGE_PS

uniform sampler2D albedo_tex;
in vec2 texcoord;
in vec4 offset[3];
in vec4 dummy2;
void main()
{
	gl_FragColor = SMAAColorEdgeDetectionPS(texcoord, offset, albedo_tex);
}


#endif


#ifdef BLEND_VS
out vec2 texcoord;
out vec2 pixcoord;
out vec4 offset[3];
out vec4 dummy2;
void main()
{
  texcoord = gl_MultiTexCoord0.xy;
  vec4 dummy1 = vec4(0);
  SMAABlendingWeightCalculationVS(dummy1, dummy2, texcoord, pixcoord, offset);
  gl_Position = ftransform();
}

#endif


#ifdef BLEND_PS
uniform sampler2D edge_tex; 
uniform sampler2D area_tex; 
uniform sampler2D search_tex; 
in vec2 texcoord;
in vec2 pixcoord; 
in vec4 offset[3];
in vec4 dummy2;
void main()
{
  gl_FragColor = SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, edge_tex, area_tex, search_tex, ivec4(0));
}

#endif


#ifdef NEIGHBORHOOD_VS
out vec2 texcoord;
out vec4 offset[2];
out vec4 dummy2;
void main()
{
  texcoord = gl_MultiTexCoord0.xy;
  vec4 dummy1 = vec4(0);
  SMAANeighborhoodBlendingVS(dummy1, dummy2, texcoord, offset);
  gl_Position = ftransform();
}

#endif

#ifdef NEIGBORHOOD_PS
uniform sampler2D albedo_tex;
uniform sampler2D blend_tex;
in vec2 texcoord;
in vec4 offset[2];
in vec4 dummy2;
void main()
{
  gl_FragColor = SMAANeighborhoodBlendingPS(texcoord, offset, albedo_tex, blend_tex);
}

#endif
