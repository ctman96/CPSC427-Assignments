#version 330

in vec4 vcolor;

// Output color
layout(location = 0) out  vec4 out_color;

void main()
{
    out_color = vcolor;
}
