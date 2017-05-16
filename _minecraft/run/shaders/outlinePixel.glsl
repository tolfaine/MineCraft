varying vec3 normal;
varying vec3 vertex_to_light_vector;
varying vec4 color;

uniform sampler2D Texture0;
uniform float screen_width;
uniform float screen_height;

uniform float ambientLevel;
uniform float elapsed;
uniform vec3 eyePosition;

void main()
{
    // X directional search matrix.
    mat3 GX = mat3( -1.0, 0.0, 1.0,
                    -2.0, 0.0, 2.0,
                    -1.0, 0.0, 1.0 );
    // Y directional search matrix.
    mat3 GY =  mat3( 1.0,  2.0,  1.0,
                     0.0,  0.0,  0.0,
                    -1.0, -2.0, -1.0 );
	vec4  fSumX = vec4( 0.0,0.0,0.0,0.0 );
    vec4  fSumY = vec4( 0.0,0.0,0.0,0.0 );
    vec4 fTotalSum = vec4( 0.0,0.0,0.0,0.0 );
	    // Findout X , Y index of incoming pixel
    // from its texture coordinate.
    float fXIndex = gl_TexCoord[0].s * screen_width;
    float fYIndex = gl_TexCoord[0].t * screen_height;



	gl_FragColor = vec4( 0.0,0.0,0.0,0.0 );

	 float fTempXx = ( fXIndex + 1.0 + 0.5) / screen_width ;
	float fTempYy = ( fYIndex + 1.0 + 0.5) / screen_height ;
//	gl_FragColor = texture2D( Texture0, vec2(  fTempXx,  fTempYy ));
	


	    /* image boundaries Top, Bottom, Left, Right pixels*/
    if( ! ( fYIndex < 1.0 || fYIndex > screen_height - 1.0 || 
            fXIndex < 1.0 || fXIndex > screen_width - 1.0 ))
    {

        // X Directional Gradient calculation.
        for(float I=-1.0; I<=1.0; I = I + 1.0)
        {
            for(float J=-1.0; J<=1.0; J = J + 1.0)
            {
                float fTempX = ( fXIndex + I + 0.5 ) / screen_width ;
                float fTempY = ( fYIndex + J + 0.5 ) / screen_height ;
                vec4 fTempSumX = texture2D( Texture0, vec2( fTempX, fTempY ));
                fSumX = fSumX + ( fTempSumX * vec4( GX[int(I+1.0)][int(J+1.0)],
                                                    GX[int(I+1.0)][int(J+1.0)],
                                                    GX[int(I+1.0)][int(J+1.0)],
                                                    GX[int(I+1.0)][int(J+1.0)]));
				
            }
        }

        { // Y Directional Gradient calculation.
            for(float I=-1.0; I<=1.0; I = I + 1.0)
            {
                for(float J=-1.0; J<=1.0; J = J + 1.0)
                {
                    float fTempX = ( fXIndex + I + 0.5 ) / screen_width ;
                    float fTempY = ( fYIndex + J + 0.5 ) / screen_height ;
                    vec4 fTempSumY = texture2D( Texture0, vec2( fTempX, fTempY ));
                    fSumY = fSumY + ( fTempSumY * vec4( GY[int(I+1.0)][int(J+1.0)],
                                                        GY[int(I+1.0)][int(J+1.0)],
                                                        GY[int(I+1.0)][int(J+1.0)],
                                                        GY[int(I+1.0)][int(J+1.0)]));
                }
            }
            // Combine X Directional and Y Directional Gradient.
            vec4 fTem = fSumX * fSumX + fSumY * fSumY;
            fTotalSum = sqrt( fTem );
        }
    }

	float i00   = texture2D(Texture0, vec2( fXIndex + 0.5, fYIndex +0.5 )).r;
    float im1m1 = texture2D(Texture0, vec2( fXIndex + 0.5 - 1.0, fYIndex +0.5 -1.0)).r;
    float ip1p1 = texture2D(Texture0, vec2( fXIndex + 0.5 + 0.1, fYIndex +0.5 +0.1 )).r;
    float im1p1 = texture2D(Texture0, vec2( fXIndex + 0.5 - 1.0, fYIndex +0.5 +1.0)).r;
    float ip1m1 = texture2D(Texture0, vec2( fXIndex + 0.5 + 1.0, fYIndex +0.5 -1.0)).r;
    float im10 = texture2D(Texture0, vec2( fXIndex + 0.5 - 1.0, fYIndex +0.5 )).r;
    float ip10 = texture2D(Texture0, vec2( fXIndex + 0.5 +1.0, fYIndex +0.5 )).r;
    float i0m1 = texture2D(Texture0, vec2( fXIndex + 0.5, fYIndex +0.5 -1.0 )).r;
    float i0p1 = texture2D(Texture0, vec2( fXIndex + 0.5, fYIndex +0.5 +1.0 )).r;
    float h = -im1p1 - 2.0 * i0p1 - ip1p1 + im1m1 + 2.0 * i0m1 + ip1m1;
    float v = -im1m1 - 2.0 * im10 - im1p1 + ip1m1 + 2.0 * ip10 + ip1p1;

	float mag = length(vec2(h, v));	
	

	vec3 normalized_normal = normalize(normal);
	vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);
//	gl_FragColor = color * (DiffuseTerm*(1-ambientLevel) + ambientLevel);
//	gl_FragColor.a = color.a;

	fTotalSum = mix( fTotalSum, texture2D( Texture0, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t)), 0.5);
	//gl_FragColor = ( fTotalSum );

	fXIndex = gl_TexCoord[0].s * screen_width;
	fYIndex = gl_TexCoord[0].t * screen_height;

    if( ! ( fYIndex < 1.0 || fYIndex > screen_height - 1.0 || 
            fXIndex < 1.0 || fXIndex > screen_width - 1.0 ))
    {


		 float fTempXx = ( fXIndex + 1.0 + 0.5) / screen_width ;
		float fTempYy = ( fYIndex + 1.0 + 0.5) / screen_height ;
		//gl_FragColor = texture2D( Texture0, vec2(  fTempXx,  fTempYy ));
	//	gl_FragColor = vec4( 0.8,0.8,0.0,0.0 );

	}


	fXIndex = gl_TexCoord[0].s * screen_width;
	fYIndex = gl_TexCoord[0].t * screen_height;

	gl_FragColor = texture2D( Texture0, vec2(  fXIndex,  fYIndex ));


}