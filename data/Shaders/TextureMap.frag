#version 330

in vec2 UV;		// texture coord (interpolated per fragment)
in vec4 vtxCol;
uniform sampler2D ts_diffuse; // texture sampler (passed by application)
uniform vec4 uModColor;

out vec4 color;	// final fragment color
void main(){
	// Output color = color of the texture at the specified UV
	color = texture( ts_diffuse, UV ); // textured


	if(color.a <= 0.05)
		discard;// discard the pixel to avoid writing to the depth buffer. 

	color *= uModColor;
}