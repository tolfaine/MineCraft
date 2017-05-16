uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform float screen_width;
uniform float screen_height;

float LinearizeDepth(float z)
{
	float n = 0.5; // camera z near
  	float f = 10000.0; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

//Sobel Edge Detection

void main (void)
{
	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );
	float depth = texture2D( Texture1 , vec2( gl_TexCoord[0] ) ).r;	
	
	depth = LinearizeDepth(depth);

	gl_FragColor = color;

    mat3 kernelX = mat3( -1.0, 0.0, 1.0,
                    -2.0, 0.0, 2.0,
                    -1.0, 0.0, 1.0 );

    mat3 kernelY =  mat3( 1.0,  2.0,  1.0,
                     0.0,  0.0,  0.0,
                    -1.0, -2.0, -1.0 );

    vec4  pixelX = vec4( 0.0,0.0,0.0,0.0 );
    vec4  pixelY = vec4( 0.0,0.0,0.0,0.0 );
    vec4 totalValue = vec4( 0.0,0.0,0.0,0.0 );

    float fXIndex = gl_TexCoord[0].s * screen_width;
    float fYIndex = gl_TexCoord[0].t * screen_height;

	int tempI = -1;
	int tempJ = -1;

    if( ! ( fYIndex < 1.0 || fYIndex > screen_height - 1.0 || 
            fXIndex < 1.0 || fXIndex > screen_width - 1.0 ))
    {
        for(float I=-1.0; I<=1.0; I = I + 1.0)
        {
            for(float J=-1.0; J<=1.0; J = J + 1.0)
            {
                float fTempX = ( fXIndex + I + 0.5 ) / screen_width ;
                float fTempY = ( fYIndex + J + 0.5 ) / screen_height ;
                vec4 fTempSumX = texture2D( Texture0, vec2( fTempX, fTempY ));

				// Coordonnées dans matrice. Parce que kenerl[-1][-1] ça marche pas trop :)
				tempI = I+1.0;
				tempJ = J+1.0 ;

                pixelX = pixelX + ( fTempSumX * vec4( kernelX[tempI][tempJ],
                                                    kernelX[tempI][tempJ],
                                                    kernelX[tempI][tempJ],
                                                    kernelX[tempI][tempJ]));
            }
        }

        { 
            for(float I=-1.0; I<=1.0; I = I + 1.0)
            {
                for(float J=-1.0; J<=1.0; J = J + 1.0)
                {
                    float fTempX = ( fXIndex + I + 0.5 ) / screen_width ;
                    float fTempY = ( fYIndex + J + 0.5 ) / screen_height ;
                    vec4 fTempSumY = texture2D( Texture0, vec2( fTempX, fTempY ));

				// Coordonnées dans matrice. Parce que kenerl[-1][-1] ça marche pas trop :)
					tempI = I+1.0;
					tempJ = J+1.0 ;

                    pixelY = pixelY + ( fTempSumY * vec4( kernelY[tempI][tempJ],
                                                        kernelY[tempI][tempJ],
                                                        kernelY[tempI][tempJ],
                                                        kernelY[tempI][tempJ]));
                }
            }

            vec4 fTem = pixelX * pixelX + pixelY * pixelY;
            totalValue = sqrt( fTem );
        }
    }

	totalValue = mix( totalValue, texture2D( Texture0, vec2( gl_TexCoord[0].s, gl_TexCoord[0].t)), 0.7);
	//totalValue = vec4( 1.0,1.0,1.0,1.0) - totalValue;

	gl_FragColor = ( totalValue );

}