# ShaderWorld
Boat rolling on the waves in OpenGL

Basic OpenGL instance provided. 
Shader code written in GLSL used in a Uni Assignment. The scene depicts a boat stuck in a heavy storm, rolling over the big and vicious waves. 

Created with OpenGL, the scene consists of a large and high vertex plane, that via the vertex shader is subjected to sin wave movement, with a smaller perlin noise based transform to create choppiness. The boat model shader calculates the tangent line of the sin wave at it given point, and rotates it around the appropriate axis to simulate it floating atop the waves.​

The fragment shader of the plane uses the vertical height to either darken or lighten the blue colour of the waves, (kind of) emulating the light entering the water between the waves. A light source is places above the boat, and reflects upon the waves.
