#include "rendering/builtinshaders.h"

const char* SHD_DEFAULT_VS =
	"#version 330\n"
	"in vec3 vertexPosition;\n"
	"in vec2 vertexTexCoord;\n"
	"in vec4 vertexColor;\n"
	"out vec2 fragTexCoord;\n"
	"out vec4 fragColor;\n"
	"uniform mat4 mvp;\n"
	"void main()\n"
	"{\n"
	"	fragTexCoord = vertexTexCoord;\n"
	"	fragColor = vertexColor;\n"
	"	gl_Position = mvp*vec4(vertexPosition, 1.0);\n"
	"}\n"
;

const char* SHD_DEFAULT_FS =
	"#version 330\n"
	"in vec2 fragTexCoord;\n"
	"in vec4 fragColor;\n"
	"out vec4 finalColor;\n"
	"uniform sampler2D texture0;\n"
	"uniform vec4 colDiffuse;\n"
	"void main()\n"
	"{\n"
	"	vec4 texelColor = texture(texture0, fragTexCoord);\n"
	"	finalColor = texelColor * colDiffuse * fragColor;\n"
	"}\n"
;
