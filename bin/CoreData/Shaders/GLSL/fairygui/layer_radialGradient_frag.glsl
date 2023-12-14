#ifdef GL_ES
precision highp float;
#endif

uniform vec4 u_startColor;
uniform vec4 u_endColor;
uniform vec2 u_center;
uniform float u_radius;
uniform float u_expand;

#ifdef GL_ES
varying lowp vec4 v_position;
#else
varying vec4 v_position;
#endif

void main()
{
    float d = distance(v_position.xy, u_center) / u_radius;
    if (d <= 1.0)
    {
        if (d <= u_expand)
        {
            gl_FragColor = u_startColor;
        }
        else
        {
            gl_FragColor = mix(u_startColor, u_endColor, (d - u_expand) / (1.0 - u_expand));
        }
    }
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
// cbuffer PSConstants {
//     float4 u_startColor;
//     float4 u_endColor;
//     float2 u_center;
//     float u_radius;
//     float u_expand;
// };
// struct PSInput {
//     float4 Pos          : SV_POSITION;
//     float4 v_position   : TEX_COORD;
// };
// struct PSOutput {
//     float4 Color : SV_TARGET;
// };
// void main(in PSInput PSIn, out PSOutput PSOut)
// {
//     float d = distance(PSIn.v_position.xy, u_center) / u_radius;
//     float4 Color;
//     if (d <= 1.0) {
//         if (d <= u_expand) {
//             Color = u_startColor;
//         } else {
//             Color = lerp(u_startColor, u_endColor, (d - u_expand) / (1.0 - u_expand));
//         }
//     } else {
//         Color = float4(0.0, 0.0, 0.0, 0.0);
//     }
//     PSOut.Color = Color;
// }
