# Procedural Terrain Generation


Real-time Procedural Multifractal Terrain Generation developed in C++ and OpenGL, capable of generating infinite terrains using multifractal models based on **Fractal Brownian Motion** and **Perlin Noise**. It incorporates Physically-Based Rendering (PBR), image-based lighting (IBL), an HDR skybox, and volumetric fog rendering. 

![ptg](https://github.com/user-attachments/assets/896d0e9d-c9c0-49fc-9d1a-debc28df5517)

## Features

### Noise

The base noise function used is perlin noise. This function is used within a domain-warped Fractal Brownian Motion function (fBM) with multiple octaves to simulate and approximate the geomorphological and jagged features of the terrains that are caused by erosion and tectonics. However, the original version of the fBM function generates only homogeneous terrains. These terrains are overly simplistic and unrealistic, since they assume that all patches of the terrain have the same roughness. 

Nature, however, is far more complex and irregular. Real landscapes are quite heterogeneous, which is why multifractal terrain models are used. These models approximate certain erosion features, without overly compromising the elegance and computational efficiency of the original fBm model. They generate terrains where low-lying areas tend to accumulate silt and become topographically smoother, while higher regions remain jagged due to ongoing erosive processes.

![ptg2](https://github.com/user-attachments/assets/dfa97049-2cb8-4eed-824f-4a0d688f7844)

### Shading 

The terrain's shading and colors are procedurally generated, without relying on pre-made texture assets.

#### Albedo Color

The terrain consists of three main elements: Rocks, grass and snow. Multi-layered noise functions generate the albedo colors for these elements by blending various color shades. The colors are interpolated together based on altitudes: Lower altitudes feature more grass, while higher altitudes transition to snow. Noise-based functions further enhance the realism by adding dark streaks to simulate terrain cracks. This color data is precomputed during program initialization and stored in a texture for real-time sampling during rendering.


#### Normal

The normal vector at a point in the terrain is computed using the first derivative of the height function. This derivative determines the slope of the terrain, representing changes in height, and is used to compute the normal vector at each point. The derivative is analytically precomputed within the noise function and stored alongside the height data in a texture for efficient real-time sampling during rendering.


#### Roughness 

Roughness is a measure of how smooth the surface of the terrain is and is essential for computing Physically Based Rendering (PBR) and Image-Based Lighting (IBL) functions. It is estimated using the first derivative of the height function, which represents the terrain's slope and variability, and is stored alongside the height and normal data in the baked texture. 


#### Ambient Occlusion 

Ambient occlusion quantifies how much ambient light is blocked at a given point on the terrain. It is approxmiated using the second derivative of the height function, which represents the terrain's curvature or slope variation, and is essential for computing Image-Based Lighting (IBL) functions. The derivative is analytically precomputed within the noise function and stored in a texture for efficient real-time sampling during rendering.

![ptg3](https://github.com/user-attachments/assets/3731540d-3d6f-49da-a9d0-921337869346)

### Volumetric Fog

This technique simulates fog by estimating the density of the fog particles in the regions of space visibile to the camera and then uses raymarching to calculate the amount of light that reaches the camera after the physical interaction of the fog particles with the incoming light. Noise-based functions were used to estimate the fog color, the fog density, the scattering coefficient and the asymmetry parameter (g) of the Henyey-Greenstein phase function, which is a mathematical model used to describe the scattering of light in mediums such as fog, clouds and water. 

![ptg8](https://github.com/user-attachments/assets/73bf7d11-16af-45f6-93e9-4ec527d3664d)

### Infinite Terrains

As the camera moves in any of the XZ directions, more unique terrains are generated, giving the illusion of infinite terrains. It works by translating terrain patches that are behind the camera to the front and recalculating the noise to generate new unique height maps. This trick simulates infinite terrains, but keeps the total number of terrain patches constant. 

![ptg7](https://github.com/user-attachments/assets/9387499d-d74f-4834-a874-94beec913997)

## Optimizations 

### Frustum Culling 

Frustum culling is used to optimize performance by rendering only the terrain patches that are inside the view frustum of the camera. The terrain patches that are outside the camera's field of view are not rendered, reducing processing load and saving the computation that would have been otherwise wasted in noise computation and terrain shading; thus improving the FPS. 

![ptg4](https://github.com/user-attachments/assets/0ac1c019-bf1c-4fa2-a3e0-764d5d48da79)

### Baking Data

Computing noise-based height, ambient occlusion, normal, roughness and albedo every frame would be inefficient, since these data do not change from frame to frame and do not depend on external dynamic parameters such as camera position. Therefore, these data can be precomputed or baked into textures only once during the program initialization, and then are sampled from in real-time during rendering. This optimization alone significantly improved the FPS. 

![ptg9](https://github.com/user-attachments/assets/213734cf-4ba6-46bb-9050-739836ec0b53)

## License

This project is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License.  
[Read the full license here](https://creativecommons.org/licenses/by-nc-nd/4.0/).


## Installation

1. Clone the repository:
```
git clone https://github.com/AmrHMorsy/procedural-terrain-generation-opengl-.git
```
2. Navigate to the project directory: 
```
cd procedural-terrain-generation-opengl
```
3. Create a build directory: 
```
mkdir build
```
4. Navigate to the build directory: 
```
cd build
```
5. Generate makefiles using cmake: 
```
cmake ../
```
6. compile using makefile: 
```
make
```
7. Run the program: 
```
./procedural-terrain-generation-opengl
```
<br>

## References 

- Ebert, D. S., Musgrave, F. K., Peachey, D., Perlin, K., & Worley, S. (2003). *Texturing and Modeling: A Procedural Approach (3rd ed.)*. Chapter 14: A Brief Introduction to Fractals. Morgan Kaufmann Publishers.
  
- Ebert, D. S., Musgrave, F. K., Peachey, D., Perlin, K., & Worley, S. (2003). *Texturing and Modeling: A Procedural Approach (3rd ed.)*. Chapter 16: Procedural Fractals Terrains. Morgan Kaufmann Publishers.

- Scratchpixel. [Perlin Noise](https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/perlin-noise.html)