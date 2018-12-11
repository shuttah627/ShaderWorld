// Give the (minimum) version of the shading language
// that this shader requires.
#version 330  

// Speficy what precision the GPU should try and use
precision highp float;

// The vertex shader (or later stage shader, i.e,
// Geometry shader) is expected to provide
// these values for the fragments. These will now
// be for each fragment in a triangle, rather
// than per vertex (thanks to rasterization and
// interpolation). 

// Data is provided to the fragment shader in the following 
// format 
in FragmentData
{
    vec3 normal;
    vec4 colour;
    vec2 st;
    vec4 EyeSpaceLightPosition;     // Used for lighting
    vec4 EyeSpacePosition;          // Used for lighting
    vec4 EyeSpaceObjectPosition;
    vec4 ShadowCoord;               // Shodow Corrdinates
} frag;


// The output of the fragment shader is provided through
// this data element. This 'fragment' is then blended with
// any other 'fragment colours' from other primitives being
// rendered to determine the *final* pixel colour
out vec4 out_fragmentColour;

// The dimensions (resolution) of the drawing buffer
uniform vec2 u_resolution;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

// The base lighting fragment shader code.
void main()
{
  // Used to build the fragment colour
  vec4 colour = vec4 (0.0, 0.0, 0.0, 1.0);

  // The gl_FragCoords is in the range of the number of pixels
  // in the viewing window. Resize these such that they are     
  vec2 st = gl_FragCoord.xy/u_resolution.xy;
  vec2 pos = (vec2(0.5)-st)*2;

  // Set up the texture coordinates
  float s = frag.st.x;
  float t = 1 - frag.st.y;             // Fix inverted texture (vertical axis)  

  // Initialise the base material colour
  vec4 MaterialColour = vec4 (0.93, 0.58, 0.55, 1.0);     // Bright red.
  vec4 AmbientColour = vec4 (0.8, 0.8, 0.8, 1.0);

  // Set up the vectors.
  vec3 N = normalize (frag.normal);
  vec3 L = normalize (vec3 (frag.EyeSpaceLightPosition - frag.EyeSpacePosition));
  vec3 R = normalize (reflect (L, N));
  vec3 V = normalize (vec3 (frag.EyeSpaceObjectPosition) - vec3 (0, 0, 0) );

  // Lighting Parameters
  float Ka = 0.7;   //  Range [0..0.7]
  float Kd = 0.7;   //  Range [0..0.7]
  float Ks = 0.0;    //  Range [0..0.3]
  float n = 10.0;
  vec4 LightColour = vec4 (0.2, 0.8, 0.0, 1.0);
  vec4 HighlightColour = vec4 (1.0, 1.0, 1.0, 1.0);

  // ambient
  colour = colour + Ka * MaterialColour;
  // diffuse
  colour = colour + MaterialColour * (Kd * max (0.0, dot (N, L))) * LightColour;
  // Specular
  colour = colour + (Ks * pow (max (0.0, dot (R, V)), n)) * HighlightColour;

  // Assign an RGBA (red/green/blue/alpha) value to the output data element
  // Values for each component (R/G/B/A) range from [0.0 to 1.0]
  out_fragmentColour = colour;
}