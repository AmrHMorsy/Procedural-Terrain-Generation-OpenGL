# Procedural-Terrain-Generation-OpenGL


Real-time Procedural Terrain Generation developed in C++ and OpenGL, capable of generating infinite terrains using multifractal models based on **Fractal Brownian Motion** and **Perlin Noise**. It incorporates Physically-Based Rendering (PBR), image-based lighting (IBL) for enhanced realism, an HDR skybox for a dynamic and immersive sky, and volumetric fog rendering to create depth and atmospheric effects.

![ptg](https://github.com/user-attachments/assets/896d0e9d-c9c0-49fc-9d1a-debc28df5517)

## Features

### Noise

![ptg2](https://github.com/user-attachments/assets/dfa97049-2cb8-4eed-824f-4a0d688f7844)

### Shading 

![ptg3](https://github.com/user-attachments/assets/3731540d-3d6f-49da-a9d0-921337869346)

### Volumetric Fog


![ptg8](https://github.com/user-attachments/assets/73bf7d11-16af-45f6-93e9-4ec527d3664d)

### Infinite Terrains

As the camera moves in any of the XZ directions, more unique terrains are generated, giving the ilusion of infinite terrains. It works by translating terrain patches that are behind the camera to the front and recalculating the noise to generate new unique height maps. This trick simulates infinite terrains, but keeps the total number of terrain patches constant. 

![ptg7](https://github.com/user-attachments/assets/9387499d-d74f-4834-a874-94beec913997)

### Frustum Culling 

Frustum culling is used to optimize performance by rendering only the terrain patches that are inside the view frustum of the camera. The terrain patches that are outside the camera's field of view are not rendered, reducing processing load and saving the computation that would have been otherwise wasted in noise computation and terrain shading; thus improving the FPS. 

![ptg4](https://github.com/user-attachments/assets/0ac1c019-bf1c-4fa2-a3e0-764d5d48da79)

### Texture Baking 

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

- Ebert, D. S., Musgrave, F. K., Peachey, D., Perlin, K., & Worley, S. (2003). Texturing and Modeling: A Procedural Approach (3rd ed.). Chapter 14: A Brief Introduction to Fractals. Morgan Kaufmann Publishers.
  
- Ebert, D. S., Musgrave, F. K., Peachey, D., Perlin, K., & Worley, S. (2003). Texturing and Modeling: A Procedural Approach (3rd ed.). Chapter 16: Procedural Fractals Terrains. Morgan Kaufmann Publishers.
