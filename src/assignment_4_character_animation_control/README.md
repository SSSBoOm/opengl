# Assignment 4 (20 points): Character animation control

## Overview

This assignment demonstrates a third-person 3D character system using modern OpenGL.
It features animated model loading, state-machine-based animation blending, camera follow mechanics, and basic object rendering that supports future collision/intersection detection.

The character model supports idle, walk, run, and jump animations using skeletal animation blending.
A dynamic third-person camera smoothly follows the character’s position and yaw (rotation), with the ability to view the character from the front when pressing Q.

## Features

- Loads FBX/DAE animated model using Assimp
- Full skeletal animation support (bone matrices)
- Animation blending with SmoothStep easing
- State machine for transitions: Idle → Walk → Run → Jump
- Jump animation only when in idle state

## Controls

- W - Walk forward
- Shift + W - Run
- A / D - Rotate character
- Space - Jump (only in idle)
- Q (Hold) - View from front of character
- Mouse Scroll - Zoom camera
- ESC - Exit

## Visualize

| Visualize                                                                     |
| ----------------------------------------------------------------------------- |
| <img src="/.github/images/assignment_4.png" alt="assignment_4" width="900" /> |

## Demo

[![Watch Demo on YouTube](https://img.shields.io/badge/Watch%20Demo-YouTube-red?logo=youtube)](https://youtu.be/61DsuNP71bE)

## 📄 Credits

This project uses external models, libraries, assets, and tools. All rights belong to their respective owners.

| Resource                                  | Description                                | Source / Author                  | License                        |
| ----------------------------------------- | ------------------------------------------ | -------------------------------- | ------------------------------ |
| Character Model (Ch42)                    | 3D humanoid model                          | Mixamo / Adobe                   | Royalty-Free (Educational Use) |
| Idle.dae, Walk.dae, Run.dae, IdleJump.dae | Animation clips used for state transitions | Mixamo / Adobe                   | Royalty-Free (Educational Use) |
| Assimp                                    | 3D model & animation loading               | assimp.org                       | BSD License                    |
| GLM (OpenGL Mathematics)                  | Vector & matrix operations                 | g-truc / glm                     | MIT License                    |
| GLFW                                      | Window creation & keyboard input handling  | glfw.org                         | Zlib License                   |
| Glad                                      | OpenGL function loader                     | glad.dav1d.de                    | MIT License                    |
| stb_image                                 | Image / Texture loading                    | Sean Barrett                     | Public Domain (Unlicense)      |
| Model, Animator, Shader classes           | Model loading, animation blending, shaders | LearnOpenGL.com by Joey de Vries | CC-BY-NC 4.0                   |
| Camera class                              | Third-person camera system base            | LearnOpenGL.com                  | CC-BY-NC 4.0                   |
| Terrain / Ground models (if used)         | Static 3D objects (tree, rock, ground)     | Sketchfab / Polyhaven            | CC0 / CC-BY 4.0                |

---

### 🙌 Special Thanks

- **Joey de Vries** — LearnOpenGL tutorials and render engine framework
- **Adobe Mixamo** — Free character models and realistic animation motion capture
- **OpenGL Community & Stack Overflow** — Debugging, shader help, and guidance
- **GLFW, GLM, Assimp developers** — For providing high-quality open-source libraries

---

### ✍ Attribution Example

> Character model and animation assets were sourced from **Mixamo (Adobe)** and used strictly for educational purposes.  
> Rendering systems, shaders, model loading, and animation blending are based on resources from **LearnOpenGL.com by Joey de Vries (CC-BY-NC 4.0)**.  
> The project integrates **Assimp, GLFW, GLM, Glad, and stb_image**, which are open-source libraries under their respective licenses.

---
