#version 330 core
uniform mat4 ModelMatrix;
uniform Camera {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

in vec3 inPosition; 
in vec2 inTexcoord;

out vec2 fragTexcoord;

void main(void)
{
	vec4 mcPosition = vec4(inPosition, 1.0);
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * mcPosition;
	fragTexcoord = inTexcoord;
}
