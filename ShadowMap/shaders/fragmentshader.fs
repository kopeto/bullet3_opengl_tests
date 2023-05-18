#version 460 core
out vec4 FragColor;

in vec3 Position;
in vec2 TexCoords;
in vec3 Normals;



uniform sampler2D texture_diffuse;

uniform struct LightInfo
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 especular;
} light;

vec3 blinnPhong(vec3 position, vec3 n)
{
    vec3 ambient = light.ambient;
    vec3 s = normalize( light.position.xyz - position);


    float sDotN = max( dot(s,n), 0.0);
    vec3 diffuse = vec3(sDotN);
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize( v + s );
        spec = vec3( pow( max( dot(h,n), 0.0 ) , 0.7 ) );
    }
    return diffuse + spec;
}

void main()
{   
    FragColor = texture(texture_diffuse, TexCoords);
}