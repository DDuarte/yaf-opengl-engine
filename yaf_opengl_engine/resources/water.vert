uniform sampler2D map;
uniform float time;

void main() 
{ 
	vec4 offset=vec4(0.0,0.0,0.0,0.0);
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[0].t += time;
    offset.y = texture2D(map, gl_TexCoord[0].st).r;

    gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex+offset);
}