#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D   screenTexture;
uniform vec2        resolution;

void main()
{
    if( ( abs(gl_FragCoord.x - resolution.x/2) < 1.0 && abs(gl_FragCoord.y - resolution.y/2) < 15.0 ) ||
    ( abs(gl_FragCoord.y - resolution.y/2) < 1.0 && abs(gl_FragCoord.x - resolution.x/2) < 15.0 ) )
        FragColor = vec4(1.0,0,0.3,1.0);
    else
        // FragColor = texture(screenTexture, TexCoords);                           // NORMAL
        FragColor = vec4( vec3( 1.0 - texture(screenTexture, TexCoords)), 1.0 ) ;   // INVERT
}