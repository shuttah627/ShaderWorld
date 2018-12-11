// A basic vertex shader, intended to help explore light shaders. 

#version 330 core
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexST;

uniform float a;
uniform float b;
uniform float c;

uniform vec4 LightPosition;
uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ShadowMatrix;

out VertexData
{
    vec3 normal;
    vec4 colour;
    vec2 st;
    vec4 EyeSpaceLightPosition;     // Used in later steps
    vec4 EyeSpacePosition;          // Used in later steps
    vec4 EyeSpaceObjectPosition;
    vec4 WorldSpaceObjectPosition;
    vec4 ShadowCoord;               // Used in later steps
} VertexOut; 

// Vertex sahder code
void main()
{
    vec4    Position;
    vec3    Normal;

    Position = vec4 (VertexPosition, 1);

    // Calculate the normal matrix and apply to normal vector... 
    mat4 normalMatrix = transpose(inverse(ViewMatrix * WorldMatrix));

    Normal = VertexNormal;

    VertexOut.normal = (normalMatrix * vec4(Normal, 0)).xyz;
    VertexOut.colour = vec4(1.0,0.0,1.0,1.0);    // Testing - hard code colour to BLUE;
    VertexOut.st = VertexST;                    // Texture mapping coordinate as passed in

    // Populate  other important quantities, also in eye
    // coordinates. The position of the light. 
    VertexOut.EyeSpaceLightPosition =  ViewMatrix * WorldMatrix * LightPosition;
    VertexOut.EyeSpacePosition = ViewMatrix * WorldMatrix * Position;
    VertexOut.EyeSpaceObjectPosition = ViewMatrix * WorldMatrix * vec4 (0, 0, 0, 1);
    VertexOut.WorldSpaceObjectPosition = WorldMatrix * Position;

    VertexOut.ShadowCoord = ProjectionMatrix * ShadowMatrix * WorldMatrix * Position;
    VertexOut.ShadowCoord = VertexOut.ShadowCoord / VertexOut.ShadowCoord.w;
    VertexOut.ShadowCoord = (VertexOut.ShadowCoord + vec4 (1.,1.,1.,0.)) *
                               vec4 (1./2., 1./2., 1./2., 1);

    // Populate the requried output of the Vertex Shader
    gl_Position = ProjectionMatrix * ViewMatrix * WorldMatrix * Position;
}
