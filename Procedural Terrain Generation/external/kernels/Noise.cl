struct Noise {
    float value ;
    float2 first_derivative ;
    float2 second_derivative ;
} ;

__constant int p[512] = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 } ;
  
float fade( float t ) ;
float fade_first_derivative( float t ) ;
float fade_second_derivative( float t ) ;
float interpolate( float t, float a, float b ) ;
float gradient( int hash, float x, float y ) ;
struct Noise perlin_noise( float2 r ) ;
struct Noise fBM( float2 r ) ;


float fade( float t )
{
    return t * t * t * ( t * ( t * 6.0f - 15.0f ) + 10.0f ) ;
}

float fade_first_derivative( float t )
{
    return 30.0f * t * t * ( t * ( t - 2.0f ) + 1.0f ) ;
}

float fade_second_derivative( float t )
{
    return 60.0f * t * ( t * ( ( 2.0f * t ) - 3.0f ) + 1.0f ) ;
}

float interpolate( float t, float a, float b )
{
    return a + t * ( b - a ) ;
}

float gradient( int hash, float x, float y )
{
    int h = hash & 7 ;

    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;

    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v) ;
}

struct Noise perlin_noise( float2 r )
{
    struct Noise noise;

    float fx = floor(r.x);
    float fy = floor(r.y);

    int x = int(fx) & 255 ;
    int y = int(fy) & 255 ;

    r.x -= fx ;
    r.y -= fy ;

    float u = fade(r.x) ;
    float v = fade(r.y) ;

    float du = fade_first_derivative(r.x) ;
    float dv = fade_first_derivative(r.y) ;

    float ddu = fade_second_derivative(du) ;
    float ddv = fade_second_derivative(dv) ;

    int a = p[x] + y ;
    int b = p[x + 1] + y ;

    float g_a = gradient( p[a], r.x, r.y ) ;
    float g_b = gradient( p[b], r.x - 1.0f, r.y ) ;
    float g_a1 = gradient( p[a + 1], r.x, r.y - 1.0f ) ;
    float g_b1 = gradient( p[b + 1], r.x - 1.0f, r.y - 1.0f ) ;

    float i1 = interpolate(u, g_a, g_b);
    float i2 = interpolate(u, g_a1, g_b1);
    noise.value = interpolate(v, i1, i2);

    float d1 = g_b - g_a;
    float d2 = g_b1 - g_a1;
    float d3 = g_a1 - g_a;

    float f1 = d2 - d1;

    noise.first_derivative.x = du * (d1 + v * f1);
    noise.first_derivative.y = dv * (d3 + u * f1);

    noise.second_derivative.x = ddu * (d1 + v * f1);
    noise.second_derivative.y = ddv * (d3 + u * f1);

    return noise;
}

struct Noise fBM( float2 r )
{
    struct Noise noise, sample;
    float H = 0.5f;
    int num_octaves = 15 ;
    float frequency = 1.0f;
    float amplitude = 1.0f;

    noise.value = 0.0f;
    noise.first_derivative = (float2)(0.0f, 0.0f);
    noise.second_derivative = (float2)(0.0f, 0.0f);

    for (int i = 0; i < num_octaves; i++) {
        sample = perlin_noise( r * frequency ) ;
        noise.value += sample.value * amplitude ;
        noise.first_derivative += amplitude * frequency * sample.first_derivative;
        noise.second_derivative += amplitude * frequency * sample.second_derivative;
        amplitude *= H;
        frequency *= 2.0f;
    }

    return noise;
}

__kernel void compute_noise( uint N, write_only image2d_t height_map )
{
    uint i = get_global_id(0) ;
    uint j = get_global_id(1) ;

    if( ( i < N ) && ( j < N ) ){
        
        float u = (float)i / (float)(N-1) ;
        float v = (float)j / (float)(N-1) ;
//        float random_offset = 0.01f; // Small perturbation
//        float x = (float)i * 12.9898 + (float)j * 78.233 ;
//        float y = (float)j * 96.8467 + (float)i * 12.9898 ;
//        float u = (float)i / (N - 1) + random_offset * sin( x ) * 43758.5453 ;
//        float v = (float)j / (N - 1) + random_offset * sin( y ) * 43758.5453 ;

        struct Noise noise = fBM( float2((float)(i), (float)(j)) ) ;
        float height = u ;
        height = (height+1.0f) / 2.0f ;
        float3 normal = (float3)( -noise.first_derivative.x, 1.0f, -noise.first_derivative.y ) ;
        float ao = clamp( 1.0f - (float)length( noise.second_derivative ), 0.0f, 1.0f ) ;
        float4 data = (float4)( height, normal.x, normal.z, ao ) ;
        write_imagef( height_map, (int2)(i,j), data ) ;
    }
}
