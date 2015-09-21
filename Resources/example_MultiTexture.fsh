
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_texture1;
uniform float u_interpolate;

void main()
{
  vec2 uv = v_texCoord;
  uv.x = uv.x + 0.001 * sin(CC_Time[1] + uv.y * 10.0);
  
  vec4 color1 = texture2D(CC_Texture0, uv);
  vec4 color2 = texture2D(u_texture1, uv);
  gl_FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
}

