#version 150

out vec4            oColor;
in vec4             Color;
uniform sampler2D   uTex0;
in vec2             TexCoord;
uniform float       uAperture;

const float PI = 3.1415926535;

void main(void)
{
    oColor = texture( uTex0, TexCoord.st ) * Color;
    return;
    
    float apertureHalf = 0.5 * uAperture * (PI / 180.0);
    
    // This factor ajusts the coordinates in the case that
    // the aperture angle is less than 180 degrees, in which
    // case the area displayed is not the entire half-sphere.
    float maxFactor = sin(apertureHalf);
    
    // The st factor takes into account the situation when non-pot
    // textures are not supported, so that the maximum texture
    // coordinate to cover the entire image might not be 1.
    vec2 stFactor = vec2(1.0f, 1.0f);
    vec2 pos = (2.0 * TexCoord.st * stFactor - 1.0);
    
    float l = length(pos);
    
    if (l > 1.0)
    {
        oColor = vec4(0, 0, 0, 1);
    }
    else
    {
        float x = maxFactor * pos.x;
        float y = maxFactor * pos.y;
        
        float n = length(vec2(x, y));
        
        float z = sqrt(1.0 - n * n);
        
        float r = atan(n, z) / PI;
        
        float phi = atan(y, x);
        
        float u = r * cos(phi) + 0.5;
        float v = r * sin(phi) + 0.5;
        
        oColor = texture( uTex0, vec2(u, v) / stFactor ) * Color;
    }
}