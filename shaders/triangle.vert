#version 450

void main()
{
    //const array of positions
    const vec3 positions[3] = vec3[3](
        vec3(1.f, 1.f, 0.0f),
        vec3(-1.f, 1.f, 0.0f),
        vec3(0.f, -1.f, 0.0f)
    );

    //output
    gl_Position = vec4(positions[gl_VertexIndex], 1.0f);
}