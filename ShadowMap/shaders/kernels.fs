#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D   screenTexture;
uniform vec2        resolution;

const float offset = 1.0 / 300.0;

void main()
{
    if( ( abs(gl_FragCoord.x - resolution.x/2) < 1.0 && abs(gl_FragCoord.y - resolution.y/2) < 15.0 ) ||
    ( abs(gl_FragCoord.y - resolution.y/2) < 1.0 && abs(gl_FragCoord.x - resolution.x/2) < 15.0 ) )
        FragColor = vec4(1.0,0,0.3,1.0);
    else
    {
        vec2 offsets[9] = vec2[](
            vec2( -offset, offset),
            vec2( 0.0f   , offset),
            vec2( offset , offset),

            vec2(-offset, 0.0f   ),
            vec2( 0.0f  , 0.0f   ),
            vec2( offset, 0.0f   ),
            
            vec2(-offset,-offset),
            vec2( 0.0f  ,-offset),
            vec2( offset,-offset)
        );

        float kernel[9] = float[](
            -1,-1,-1,
            -1, 9,-1,
            -1,-1,-1
        );

        float blur_kernel[9] = float[](
            1.0/16.0,2.0/16.0,1.0/16.0,
            2.0/16.0,4.0/16.0,2.0/16.0,
            1.0/16.0,2.0/16.0,1.0/16.0
        );
        float edges_kernel[9] = float[](
            1,1,1,
            1,8,1,
            1,1,1
        );

        vec3 sampleTex[9];
        for(int i = 0; i<9; ++i)
        {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.xy + offsets[i]));
        }

        vec3 col = vec3(1.0);
        for(int i = 0; i<9; ++i)
            col+= sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);
    }

}