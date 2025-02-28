# Drone-Delivery-Rush
Drone Delivery Rush is a 3D game where you pilot a drone to deliver packages across a city. Navigate through buildings, drop off deliveries accurately, and earn points for each successful package.

This project is a 3D drone simulation built using OpenGL and the glm library. The simulation includes various objects such as buildings, trees, and terrain, with interactive drone movement and collision detection.

Features

3D Drone Movement: The drone can move in all directions, including ascending and descending.

Terrain and Objects: The simulation includes terrain, buildings, trees, and a package delivery system.

Collision Detection: Ensures the drone does not pass through buildings or trees.

Package Delivery System: The drone can pick up and deliver packages.

Minimap Camera: A secondary camera provides an overhead view for better navigation.

Controls

W - Move forward

S - Move backward

A - Move left

D - Move right

Q - Descend

E - Ascend

Code Structure

Tema2.h / Tema2.cpp - Main simulation logic

Camera2.h / Camera2.cpp - Camera management

Mesh Creation - Functions to generate terrain, buildings, trees, and other objects

Collision Detection - checkAABBCollision() ensures objects do not overlap

Shaders - Custom shaders for rendering
