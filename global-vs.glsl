#version 330 core
uniform mat4 ModelMatrix;
uniform Camera {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexcoord;

out vec2 fragTexcoord;
out vec3 Position;
out vec3 Normal;
out vec3 Eye;


void main(void)
{
	Position = vec3(ModelMatrix * vec4(inPosition, 1.0));
	Normal = normalize(mat3(transpose(inverse(ModelMatrix))) * inNormal);
	Eye = ViewMatrix[3].xyz;
	fragTexcoord = inTexcoord;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1.0);
}