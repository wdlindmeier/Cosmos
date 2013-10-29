#version 150

uniform mat4        uModelViewProjection;
uniform sampler2D   uTex0;

in vec4         vPosition;
in vec2         vTexCoord0;
in vec4         vColor;
out lowp vec4	Color;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main( void )
{
    vec4 pos = vPosition;
    
    vec2 heightPosition = vec2(pos.x, 1.0-pos.z);
    vec4 heightSample = texture(uTex0, heightPosition);
    
    float audioValue = heightSample.x;
    // Threshold
    // if (audioValue < 0.2) audioValue = 0.0;
    
    float scalarZ = (1.0-pos.z);
    float scalarX = (0.5 + pos.x) / 1.0;
    float maxAudioDist = 0.5;
    float audioDist = audioValue * maxAudioDist * scalarZ;
    float dist = 0.5 - audioDist;
    float rads = radians(-90.0 + (scalarX * 360.0));
    float x = cos(rads);
    float y = sin(rads);
    vec4 shaderPos = pos;
    shaderPos.x = (x * dist) + 0.5;
    shaderPos.y = y * dist;
    
    vec3 hsvColor = vec3(scalarX, 1.0, 1.0);
    vec3 rgbColor = hsv2rgb(hsvColor);

	gl_Position	= uModelViewProjection * shaderPos;
	Color = vec4(rgbColor, (audioValue * 0.1) * scalarZ);
}
