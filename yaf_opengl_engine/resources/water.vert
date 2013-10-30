uniform sampler2D map;
uniform float time;

void main() 
{ 
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[0].t += time;
    gl_Vertex.y = texture2D(map, gl_TexCoord[0].st).r;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
