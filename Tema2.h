#pragma once

#include <string>
#include <unordered_map>
#include "components/simple_scene.h"
#include "components/transform.h"
#include "Camera.h"

namespace m1 {
    class Tema2 : public gfxc::SimpleScene {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void RenderMinimap();
        void RenderDroneOrtho(const glm::vec3& position, float rotationAngle);
        void RenderBuildingOrtho(const glm::vec3& position, const glm::vec3& scale);
        void RenderTreeOrtho(const glm::vec3& position, const glm::vec3& trunkScale, const glm::vec3& leavesScale);
        void RenderPackageOrtho(const glm::vec3& position);
        void RenderDeliveryPlatformOrtho(const glm::vec3& position);
        void RenderDroneArrowOrtho(const glm::vec3& position, float rotationAngle);
        void RenderDroneArrow(const glm::vec3& position, float rotationAngle, const glm::mat4& view, const glm::mat4& projection);
        void RenderDroneArrow(const glm::vec3& position, float rotationAngle);
        void RenderBuildingMinimap(const glm::vec3& position, const glm::vec3& scale);
        void RenderTreeMinimap(const glm::vec3& position, const glm::vec3& trunkScale, const glm::vec3& leavesScale);
        void RenderDroneMinimap(const glm::vec3& position, float rotationAngle);
        void FrameEnd() override;

        Mesh* CreateCube(const std::string& name, const glm::vec3& center, float length);
        Mesh* Tema2::CreateColoredCube(const std::string& name, const glm::vec3& center, float length, const glm::vec3& color);
        Mesh* Tema2::CreatePyramid(const std::string& name, const glm::vec3& center, float length, const glm::vec3& color);
        Mesh* Tema2::CreateTerrain(const std::string& name, int rows, int cols, float spacing, glm::vec3 color);
        Mesh* Tema2::CreateCylinder(const std::string& name, float radius, float height, const glm::vec3& color);
        Mesh* Tema2::CreateCone(const std::string& name, float radius, float height, const glm::vec3& color);
        Mesh* CreatePlatform(const std::string& name, float radius, float height, const glm::vec3& color);
        void Tema2::RenderTree(const glm::vec3& position, const glm::vec3& trunkScale, const glm::vec3& leavesScale);
        //void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderDrone(const glm::vec3& position, float rotationAngle);
        void RenderMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int objectID);
        void RenderMeshOrtho(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int objectID);
        void Tema2::RenderBuilding(const glm::vec3& position, const glm::vec3& scale);
        void RenderScene(implemented::Camera2* renderCamera, const glm::mat4& projectionMatrix);
        void DrawMinimapBackground(int x, int y, int width, int height);
        void RenderDroneIconForMinimap(const glm::vec3& position);


        std::vector<glm::vec3> Tema2::GenerateRandomPositions(int count, float terrainSize, float minDistance, const std::vector<glm::vec3>& excludePositions);
        struct AABB {
            glm::vec3 min;
            glm::vec3 max;
        };

        AABB getAABB(const glm::vec3& position, const glm::vec3& size);
        bool checkAABBCollision(const AABB& a, const AABB& b);

        void OnInputUpdate(float deltaTime, int mods) override;
        float fract(float value);
        float Tema2::noise(glm::vec2 pos);
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;

     

        glm::vec3 dronePosition;
        float droneRotation;
        float propellerRotation;
        float distanceToDrone; 
		implemented::Camera2* camera;
        glm::mat4 projectionMatrix;
        std::vector<glm::vec3> buildingPositions;
		std::vector<glm::vec3> treePositions;
        implemented::Camera2* minimapCamera;
        glm::mat4 minimapProjectionMatrix;

        int minimapX;
        int minimapY;
        int minimapWidth;
        int minimapHeight;
        glm::vec3 packagePosition;
        std::vector<glm::vec3> packagePositions;
        glm::vec3 platformPosition; 
        glm::vec3 deliveryDestination; 
        bool platformVisible = true;
        bool hasPackage;
        bool pickupVisible;
        bool deliveryVisible;
        int packagesDelivered=0;
		int score = 0;
        glm::vec3 arrowOffset = glm::vec3(-1.0f, 2.0f, -2.0f); 
        glm::vec3 arrowPosition = dronePosition + arrowOffset;





    };
}
