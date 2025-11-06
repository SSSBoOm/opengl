# Assignment 3 (20 points): Loading 3D model, camera following, object intersection

## Overview

This project implements an interactive 3D Maze Game using modern OpenGL.
The player explores a procedurally generated maze rendered with textured walls, realistic lighting, and a dynamic third-person camera that follows the player’s movement and rotation.
The environment includes basic wall collision detection and camera tracking for an immersive gameplay experience.

## Features

- Procedural maze generation using depth-first search (DFS) backtracking
- Textured 3D walls and ground with adjustable height and size
- Third-person follow camera with smooth rotation and pitch angle (~60°)
- Realistic player model loaded with Assimp (.obj / .fbx)
- Full movement collision detection against maze walls
- Configurable movement speed and camera offsets

## Controls

- W / A / S / D — Move player forward, left, backward, right
- Q - Toggle plat animation
- ← / → (Arrow Keys) — Rotate player direction
- Esc — Exit the game

## Visualize

| Visualize                                                                     |
| ----------------------------------------------------------------------------- |
| <img src="/.github/images/assignment_3.png" alt="3d sculpture" width="900" /> |

## Demo

[![Watch Demo on YouTube](https://img.shields.io/badge/Watch%20Demo-YouTube-red?logo=youtube)](https://youtu.be/mII2cEHWfHA)
