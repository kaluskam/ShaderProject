// STEP 4 - Animation with dynamic variables.
// Copyright (c) 2015-23 by Carlos Martinho

#version 330 core

uniform mat4 ModelMatrix;
uniform Camera {
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;	
};

uniform float Time0_1;
const float PI = 3.141592;

in vec3 inPosition;

void main(void)
{
	vec4 mcPosition = vec4(inPosition, 1.0);
	mcPosition.y = sin(5.0 * mcPosition.x + 2.0 * PI * Time0_1) * 0.1;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * mcPosition;
}

//	v.y = sin(angle(v.x) + offset) * amplitude;
