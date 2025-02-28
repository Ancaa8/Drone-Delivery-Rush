#include "Tema2.h"
#include <vector>
#include <string>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace m1;
using namespace implemented;

Tema2::Tema2() {
    dronePosition = glm::vec3(0, 0.5f, 0);
    droneRotation = 0;                 
    propellerRotation = 0;


}

Tema2::~Tema2() {}

void Tema2::Init() {

    hasPackage = false;      
    pickupVisible = true;     
    deliveryVisible = false;


    Mesh* grayCube = CreateColoredCube("grayCube", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));
    meshes[grayCube->GetMeshID()] = grayCube;

    Mesh* blackCube = CreateColoredCube("blackCube", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    meshes[blackCube->GetMeshID()] = blackCube;

    Mesh* terrain = CreateTerrain("terrain", 100, 100, 1.0f, glm::vec3(0.3f, 0.8f, 0.3f));
    meshes[terrain->GetMeshID()] = terrain;

    Mesh* treeTrunk = CreateCylinder("treeTrunkArrow", 0.2f, 2.0f, glm::vec3(0.55f, 0.27f, 0.07f));
    meshes[treeTrunk->GetMeshID()] = treeTrunk;

    Mesh* redCone = CreateCone("redCone", 0.5f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    meshes[redCone->GetMeshID()] = redCone;


    meshes["buildingBody"] = CreateColoredCube("buildingBody", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));
    meshes["buildingRoof"] = CreatePyramid("buildingRoof", glm::vec3(0, 0, 0), 1.0f, glm::vec3(0.8f, 0.2f, 0.2f));
    meshes["treeTrunk"] = CreateCylinder("treeTrunk", 0.9f, 2.0f, glm::vec3(0.55f, 0.27f, 0.07f));
    meshes["treeLeaves"] = CreateCone("treeLeaves", 1.0f, 1.5f, glm::vec3(0.0f, 0.5f, 0.0f));

	camera = new Camera2();
    camera->Set(glm::vec3(0, 10.0f, 20), glm::vec3(0, 0.0f, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.1f, 10000.0f);

  
	minimapCamera = new Camera2();
	minimapCamera->Set(glm::vec3(0, 50.0f, 0), glm::vec3(0, 0.0f, 0), glm::vec3(0, 0, -1));
	minimapProjectionMatrix = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.8f, 100.0f);

    buildingPositions = GenerateRandomPositions(20, 100.0f, 5.0f, {});
    treePositions = GenerateRandomPositions(50, 100.0f, 5.0f, buildingPositions);

    Shader* myShader = new Shader("MyShader");
    myShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexColor.glsl"), GL_VERTEX_SHADER);
    myShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentColor.glsl"), GL_FRAGMENT_SHADER);
    myShader->CreateAndLink();
    shaders[myShader->GetName()] = myShader;

    Shader* objectShader = new Shader("ObjectShader");
    objectShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "ObjectVertex.glsl"), GL_VERTEX_SHADER);
    objectShader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "ObjectFragment.glsl"), GL_FRAGMENT_SHADER);
    objectShader->CreateAndLink();
    shaders[objectShader->GetName()] = objectShader;

    Mesh* platform = CreatePlatform("platform", 2.0f, 15.0f, glm::vec3(0.5f, 0.2f, 0.2f));
    meshes[platform->GetMeshID()] = platform;

    packagePosition = GenerateRandomPositions(1, 100.0f, 10.0f, {})[0];
    packagePosition.y = 3.5f; 
    packagePositions.push_back(packagePosition);

    platformPosition = packagePosition;
    platformPosition.y = 1.0f;

    deliveryDestination = GenerateRandomPositions(1, 100.0f, 10.0f, {})[0];
    deliveryDestination.y = 1.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


void Tema2::Update(float deltaTimeSeconds) {
    propellerRotation += deltaTimeSeconds * 10;

    dronePosition.y = camera->GetTargetPosition().y;
    dronePosition.x = camera->GetTargetPosition().x;
    dronePosition.z = camera->GetTargetPosition().z;

    glm::mat4 terrainModelMatrix = glm::mat4(1);
    terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(-50, 0, -50));
    RenderMesh2(meshes["terrain"], shaders["MyShader"], terrainModelMatrix, -1);

    if (pickupVisible) {
        glm::mat4 platformModelMatrix = glm::mat4(1);
        platformModelMatrix = glm::translate(platformModelMatrix, platformPosition);
        
        if (!hasPackage) {
            glm::mat4 packageModelMatrix = glm::mat4(1);
            packageModelMatrix = glm::translate(packageModelMatrix, packagePosition);
            packageModelMatrix = glm::scale(packageModelMatrix, glm::vec3(1.0f));
            RenderMesh2(meshes["grayCube"], shaders["ObjectShader"], packageModelMatrix, 7);


            AABB droneBox = getAABB(dronePosition, glm::vec3(1.0f, 0.5f, 1.0f));
            AABB packageBox = getAABB(packagePosition, glm::vec3(0.5f, 0.5f, 0.5f));
            if (checkAABBCollision(droneBox, packageBox)) {
                hasPackage = true;
                pickupVisible = false;
                deliveryVisible = true;
                deliveryDestination = GenerateRandomPositions(1, 100.0f, 5.0f, {})[0];
                deliveryDestination.y = 0.5f;
            }
        }
    }

    if (hasPackage) {
        packagePosition = dronePosition - glm::vec3(0, 0.4f, 0);
        glm::mat4 packageModelMatrix = glm::mat4(1);
        packageModelMatrix = glm::translate(packageModelMatrix, packagePosition);
        packageModelMatrix = glm::scale(packageModelMatrix, glm::vec3(0.6f));
        RenderMesh2(meshes["grayCube"], shaders["ObjectShader"], packageModelMatrix, 7);
    }

    if (deliveryVisible) {
        glm::mat4 deliveryModelMatrix = glm::mat4(1);
        deliveryModelMatrix = glm::translate(deliveryModelMatrix, deliveryDestination);
        RenderMesh2(meshes["platform"], shaders["ObjectShader"], deliveryModelMatrix, 8);

        float radius = 2.0f;
        float height = 15.0f;

        glm::vec3 deliveryCenter = deliveryDestination + glm::vec3(0, height / 2.0f, 0);
        glm::vec3 deliverySize = glm::vec3(radius * 2.0f, height, radius * 2.0f);

        AABB deliveryBox = getAABB(deliveryCenter, deliverySize);
        AABB droneBox = getAABB(dronePosition, glm::vec3(1.0f, 0.5f, 1.0f));

        if (checkAABBCollision(droneBox, deliveryBox)) {
            if (hasPackage) {
                hasPackage = false;
                deliveryVisible = false;
                pickupVisible = true;
                platformPosition = GenerateRandomPositions(1, 100.0f, 5.0f, {})[0];
                platformPosition.y = 3.0f;
                packagePosition = platformPosition + glm::vec3(0, 0.5f, 0);

                packagesDelivered++; 
				score += 100;
                std::cout << "Pachete livrate: " << packagesDelivered << std::endl; 
				std::cout << "Scor: " << score << std::endl;

            }
        }
    }


    for (const auto& pos : buildingPositions) {
        RenderBuilding(pos, glm::vec3(2.0f, 5.0f, 2.0f));
    }

    for (const auto& pos : treePositions) {
        RenderTree(pos, glm::vec3(0.3f, 2.0f, 0.3f), glm::vec3(1.0f, 1.5f, 1.0f));
    }

    RenderDrone(dronePosition, droneRotation);
    glm::vec3 arrowPosition = dronePosition + glm::vec3(0.0f, 0.5f, -6.0f);
    glm::vec3 targetPosition = hasPackage ? deliveryDestination : packagePosition;
    glm::vec3 directionArrow = glm::normalize(glm::vec3(targetPosition.x - arrowPosition.x, 0.0f, targetPosition.z - arrowPosition.z));
    float rotationAngleArrow = atan2(directionArrow.x, directionArrow.z);

    glm::mat4 arrowTrunkModelMatrix = glm::mat4(1);
    arrowTrunkModelMatrix = glm::translate(arrowTrunkModelMatrix, arrowPosition);
    arrowTrunkModelMatrix = glm::rotate(arrowTrunkModelMatrix, rotationAngleArrow, glm::vec3(0, 1, 0));
    arrowTrunkModelMatrix = glm::rotate(arrowTrunkModelMatrix, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    arrowTrunkModelMatrix = glm::scale(arrowTrunkModelMatrix, glm::vec3(0.3f, 1.0f, 0.3f));
    RenderMesh2(meshes["treeTrunkArrow"], shaders["ObjectShader"], arrowTrunkModelMatrix, 10);

    glm::vec3 coneBasePosition = arrowPosition + glm::vec3(0.0f, 0.0f, 0.3f);

    glm::mat4 arrowConeModelMatrix = glm::mat4(1);
    arrowConeModelMatrix = glm::translate(arrowConeModelMatrix, coneBasePosition);
    arrowConeModelMatrix = glm::rotate(arrowConeModelMatrix, rotationAngleArrow, glm::vec3(0, 1, 0));
    arrowConeModelMatrix = glm::rotate(arrowConeModelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
    arrowConeModelMatrix = glm::scale(arrowConeModelMatrix, glm::vec3(0.5f, 1.0f, 0.5f));
    RenderMesh2(meshes["redCone"], shaders["ObjectShader"], arrowConeModelMatrix, 10);


}


void Tema2::FrameStart() {
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);

}

void Tema2::FrameEnd() {

    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
    glClear(GL_DEPTH_BUFFER_BIT);

    int minimapWidth = window->GetResolution().x / 2;
    int minimapHeight = window->GetResolution().y / 2;
    int minimapX = window->GetResolution().x - minimapWidth+ 150;
    int minimapY = 0-80;
    glViewport(minimapX, minimapY, minimapWidth, minimapHeight);
    RenderMinimap();
}



void Tema2::RenderMinimap() {
    minimapCamera->Set(glm::vec3(0, 0.5f, 0) + glm::vec3(0, 50.0f, 0), glm::vec3(0, 0.5f, 0), glm::vec3(0, 0, -1));

    glm::mat4 terrainModelMatrix = glm::mat4(1);
    terrainModelMatrix = glm::translate(terrainModelMatrix, glm::vec3(-50, 0, -50));

	RenderMeshOrtho(meshes["terrain"], shaders["MyShader"], terrainModelMatrix,-1);

    for (const auto& pos : buildingPositions) {
        RenderBuildingOrtho(pos, glm::vec3(2.0f, 5.0f, 2.0f));
    }

    for (const auto& pos : treePositions) {
        RenderTreeOrtho(pos, glm::vec3(0.3f, 2.0f, 0.3f), glm::vec3(1.0f, 1.5f, 1.0f));
    }

    RenderDroneOrtho(dronePosition, droneRotation);

    if (pickupVisible) {
        RenderPackageOrtho(packagePosition);
    }

    if (deliveryVisible) {
        RenderDeliveryPlatformOrtho(deliveryDestination);
    }
}

void Tema2::RenderDroneOrtho(const glm::vec3& position, float rotationAngle) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0f)); 

    glm::mat4 arm1 = modelMatrix;
    arm1 = glm::scale(arm1, glm::vec3(1.2f, 0.5f, 0.1f));
    RenderMeshOrtho(meshes["grayCube"], shaders["ObjectShader"], arm1, 1);

    for (int i = -1; i <= 1; i += 2) {
        glm::mat4 cubeEnd1 = modelMatrix;
        cubeEnd1 = glm::translate(cubeEnd1, glm::vec3(i * 0.55f, 0.05f, 0));
        cubeEnd1 = glm::scale(cubeEnd1, glm::vec3(0.1f));
        RenderMeshOrtho(meshes["grayCube"], shaders["ObjectShader"], cubeEnd1, 1);
    }

    glm::mat4 arm2 = modelMatrix;
    arm2 = glm::rotate(arm2, glm::radians(90.0f), glm::vec3(0, 1, 0));
    arm2 = glm::scale(arm2, glm::vec3(1.2f, 0.5f, 0.1f));
    RenderMeshOrtho(meshes["grayCube"], shaders["ObjectShader"], arm2, 1);

    for (int i = -1; i <= 1; i += 2) {
        glm::mat4 cubeEnd2 = modelMatrix;
        cubeEnd2 = glm::translate(cubeEnd2, glm::vec3(0, 0.05f, i * 0.55f));
        cubeEnd2 = glm::scale(cubeEnd2, glm::vec3(0.1f));
        RenderMeshOrtho(meshes["grayCube"], shaders["ObjectShader"], cubeEnd2, 1);
    }

    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glm::mat4 propellerMatrix = modelMatrix;
            propellerMatrix = glm::translate(propellerMatrix, glm::vec3(i * 0.55f, 0.1f, j * 0.0f));
            propellerMatrix = glm::rotate(propellerMatrix, propellerRotation, glm::vec3(0, 1, 0));
            propellerMatrix = glm::scale(propellerMatrix, glm::vec3(0.02f, 0.02f, 0.3f));
            RenderMeshOrtho(meshes["blackCube"], shaders["ObjectShader"], propellerMatrix, 2);
        }
    }

    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glm::mat4 propellerMatrix = modelMatrix;
            propellerMatrix = glm::translate(propellerMatrix, glm::vec3(i * 0.0f, 0.1f, j * 0.55f));
            propellerMatrix = glm::rotate(propellerMatrix, propellerRotation, glm::vec3(0, 1, 0));
            propellerMatrix = glm::scale(propellerMatrix, glm::vec3(0.02f, 0.02f, 0.3f));
            RenderMeshOrtho(meshes["blackCube"], shaders["ObjectShader"], propellerMatrix, 2);
        }
    }
}

void Tema2::RenderBuildingOrtho(const glm::vec3& position, const glm::vec3& scale) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    RenderMeshOrtho(meshes["buildingBody"], shaders["ObjectShader"], modelMatrix, 3);

    glm::mat4 roofMatrix = glm::mat4(1);
    roofMatrix = glm::translate(roofMatrix, position + glm::vec3(0, scale.y - (scale.y / 2), 0));
    roofMatrix = glm::scale(roofMatrix, glm::vec3(scale.x, scale.y / 2, scale.z));

    RenderMeshOrtho(meshes["buildingRoof"], shaders["ObjectShader"], roofMatrix, 4);
}

void Tema2::RenderTreeOrtho(const glm::vec3& position, const glm::vec3& trunkScale, const glm::vec3& leavesScale) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(glm::mat4(1), position);
    modelMatrix = glm::scale(modelMatrix, trunkScale);
    RenderMeshOrtho(meshes["treeTrunk"], shaders["ObjectShader"], modelMatrix, 5);

    glm::mat4 leavesMatrix1 = glm::translate(glm::mat4(1), position + glm::vec3(0, trunkScale.y, 0));
    leavesMatrix1 = glm::scale(leavesMatrix1, leavesScale);

    RenderMeshOrtho(meshes["treeLeaves"], shaders["ObjectShader"], leavesMatrix1, 6);

    glm::mat4 leavesMatrix2 = glm::translate(glm::mat4(1), position + glm::vec3(0, trunkScale.y + leavesScale.y, 0));
    leavesMatrix2 = glm::scale(leavesMatrix2, leavesScale * 0.8f);

    RenderMeshOrtho(meshes["treeLeaves"], shaders["ObjectShader"], leavesMatrix2, 6);
}

void Tema2::RenderPackageOrtho(const glm::vec3& position) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(3.6f));
    RenderMeshOrtho(meshes["grayCube"], shaders["ObjectShader"], modelMatrix, 7);
}

void Tema2::RenderDeliveryPlatformOrtho(const glm::vec3& position) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
    RenderMeshOrtho(meshes["treeTrunk"], shaders["ObjectShader"], modelMatrix, 9);
}




Mesh* Tema2::CreatePyramid(const std::string& name, const glm::vec3& center, float length, const glm::vec3& color) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(center + glm::vec3(-length / 2, 0, -length / 2), color),
        VertexFormat(center + glm::vec3(length / 2, 0, -length / 2), color),
        VertexFormat(center + glm::vec3(length / 2, 0, length / 2), color),
        VertexFormat(center + glm::vec3(-length / 2, 0, length / 2), color),
        VertexFormat(center + glm::vec3(0, length, 0), color)
    };

    std::vector<unsigned int> indices = {
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4,
        0, 1, 2, 2, 3, 0
    };

    Mesh* pyramid = new Mesh(name);
    pyramid->InitFromData(vertices, indices);
    return pyramid;
}

Mesh* Tema2::CreateCube(const std::string& name, const glm::vec3& center, float length) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(center + glm::vec3(-length / 2, -length / 2, -length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
        VertexFormat(center + glm::vec3(length / 2, -length / 2, -length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
        VertexFormat(center + glm::vec3(length / 2, -length / 2, length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
        VertexFormat(center + glm::vec3(-length / 2, -length / 2, length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),

        VertexFormat(center + glm::vec3(-length / 2, length / 2, -length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
        VertexFormat(center + glm::vec3(length / 2, length / 2, -length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
        VertexFormat(center + glm::vec3(length / 2, length / 2, length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
        VertexFormat(center + glm::vec3(-length / 2, length / 2, length / 2), glm::vec3(0.5f, 0.5f, 0.5f)),
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        3, 2, 6, 6, 7, 3,
        0, 1, 5, 5, 4, 0,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1
    };

    Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);
    return cube;
}

Mesh* Tema2::CreateColoredCube(const std::string& name, const glm::vec3& center, float length, const glm::vec3& color) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(center + glm::vec3(-length / 2, -length / 2, -length / 2), color),
        VertexFormat(center + glm::vec3(length / 2, -length / 2, -length / 2), color),
        VertexFormat(center + glm::vec3(length / 2, -length / 2, length / 2), color),
        VertexFormat(center + glm::vec3(-length / 2, -length / 2, length / 2), color),

        VertexFormat(center + glm::vec3(-length / 2, length / 2, -length / 2), color),
        VertexFormat(center + glm::vec3(length / 2, length / 2, -length / 2), color),
        VertexFormat(center + glm::vec3(length / 2, length / 2, length / 2), color),
        VertexFormat(center + glm::vec3(-length / 2, length / 2, length / 2), color),
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        3, 2, 6, 6, 7, 3,
        0, 1, 5, 5, 4, 0,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1
    };

    Mesh* cube = new Mesh(name);
    cube->InitFromData(vertices, indices);
    return cube;
}

Mesh* Tema2::CreateTerrain(const std::string& name, int rows, int cols, float spacing, glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= rows; ++i) {
        for (int j = 0; j <= cols; ++j) {
            float x = j * spacing;
            float z = i * spacing;
            vertices.emplace_back(glm::vec3(x, 0.0f, z), color);
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int topLeft = i * (cols + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * (cols + 1) + j;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* terrainMesh = new Mesh(name);
    terrainMesh->InitFromData(vertices, indices);
    return terrainMesh;
}

Mesh* Tema2::CreateCylinder(const std::string& name, float radius, float height, const glm::vec3& color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int numSlices = 32;

    for (int i = 0; i <= numSlices; i++) {
        float angle = i * 2.0f * glm::pi<float>() / numSlices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.emplace_back(glm::vec3(x, 0, z), color);
        vertices.emplace_back(glm::vec3(x, height, z), color);
    }

    for (int i = 0; i < numSlices; i++) {
        int base1 = i * 2;
        int base2 = (i + 1) * 2;

        indices.push_back(base1);
        indices.push_back(base2);
        indices.push_back(base1 + 1);

        indices.push_back(base1 + 1);
        indices.push_back(base2);
        indices.push_back(base2 + 1);

        indices.push_back(base1);
        indices.push_back(base2);
        indices.push_back(numSlices * 2);

        indices.push_back(base1 + 1);
        indices.push_back(base2 + 1);
        indices.push_back(numSlices * 2 + 1);
    }

    vertices.emplace_back(glm::vec3(0, 0, 0), color);
    vertices.emplace_back(glm::vec3(0, height, 0), color);

    Mesh* cylinder = new Mesh(name);
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* Tema2::CreateCone(const std::string& name, float radius, float height, const glm::vec3& color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int numSlices = 32;

    vertices.emplace_back(glm::vec3(0, height, 0), color);

    for (int i = 0; i <= numSlices; i++) {
        float angle = i * 2.0f * glm::pi<float>() / numSlices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, 0, z), color);
    }

    for (int i = 1; i <= numSlices; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* cone = new Mesh(name);
    cone->InitFromData(vertices, indices);
    return cone;
}

Mesh* Tema2::CreatePlatform(const std::string& name, float radius, float height, const glm::vec3& color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int numSlices = 64;

    for (int i = 0; i <= numSlices; i++) {
        float angle = i * 2.0f * glm::pi<float>() / numSlices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, 0, z), color);
        vertices.emplace_back(glm::vec3(x, height, z), color);
    }

    for (int i = 0; i < numSlices; i++) {
        int base1 = i * 2;
        int base2 = (i + 1) * 2;

        indices.push_back(base1);
        indices.push_back(base2);
        indices.push_back(base1 + 1);

        indices.push_back(base1 + 1);
        indices.push_back(base2);
        indices.push_back(base2 + 1);
    }

    Mesh* platform = new Mesh(name);
    platform->InitFromData(vertices, indices);
    return platform;
}





void Tema2::RenderBuilding(const glm::vec3& position, const glm::vec3& scale) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderMesh2(meshes["buildingBody"], shaders["ObjectShader"], modelMatrix, 3);

    glm::mat4 roofMatrix = glm::mat4(1);
    roofMatrix = glm::translate(roofMatrix, position + glm::vec3(0, scale.y - (scale.y / 2), 0));
    roofMatrix = glm::scale(roofMatrix, glm::vec3(scale.x, scale.y / 2, scale.z));
    RenderMesh2(meshes["buildingRoof"], shaders["ObjectShader"], roofMatrix, 4);
}

void Tema2::RenderTree(const glm::vec3& position, const glm::vec3& trunkScale, const glm::vec3& leavesScale) {
    glm::mat4 modelMatrix = glm::mat4(1);

    modelMatrix = glm::translate(glm::mat4(1), position);
    modelMatrix = glm::scale(modelMatrix, trunkScale);
    RenderMesh2(meshes["treeTrunk"], shaders["ObjectShader"], modelMatrix, 5);

    glm::mat4 leavesMatrix1 = glm::translate(glm::mat4(1), position + glm::vec3(0, trunkScale.y, 0));
    leavesMatrix1 = glm::scale(leavesMatrix1, leavesScale);
    RenderMesh2(meshes["treeLeaves"], shaders["ObjectShader"], leavesMatrix1, 6);

    glm::mat4 leavesMatrix2 = glm::translate(glm::mat4(1), position + glm::vec3(0, trunkScale.y + leavesScale.y, 0));
    leavesMatrix2 = glm::scale(leavesMatrix2, leavesScale * 0.8f);
    RenderMesh2(meshes["treeLeaves"], shaders["ObjectShader"], leavesMatrix2, 6);
}

void Tema2::RenderDrone(const glm::vec3& position, float rotationAngle) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::vec3(0, 1, 0));

    glm::mat4 arm1 = modelMatrix;
    arm1 = glm::scale(arm1, glm::vec3(1.2f, 0.05f, 0.1f));
    RenderMesh2(meshes["grayCube"], shaders["ObjectShader"], arm1, 1);

    for (int i = -1; i <= 1; i += 2) {
        glm::mat4 cubeEnd1 = modelMatrix;
        cubeEnd1 = glm::translate(cubeEnd1, glm::vec3(i * 0.55f, 0.05f, 0));
        cubeEnd1 = glm::scale(cubeEnd1, glm::vec3(0.1f));
        RenderMesh2(meshes["grayCube"], shaders["ObjectShader"], cubeEnd1, 1);
    }

    glm::mat4 arm2 = modelMatrix;
    arm2 = glm::rotate(arm2, glm::radians(90.0f), glm::vec3(0, 1, 0));
    arm2 = glm::scale(arm2, glm::vec3(1.2f, 0.05f, 0.1f));
    RenderMesh2(meshes["grayCube"], shaders["ObjectShader"], arm2, 1);

    for (int i = -1; i <= 1; i += 2) {
        glm::mat4 cubeEnd2 = modelMatrix;
        cubeEnd2 = glm::translate(cubeEnd2, glm::vec3(0, 0.05f, i * 0.55f)); 
        cubeEnd2 = glm::scale(cubeEnd2, glm::vec3(0.1f));
        RenderMesh2(meshes["grayCube"], shaders["ObjectShader"], cubeEnd2, 1);
    }

    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glm::mat4 propellerMatrix = modelMatrix;
            propellerMatrix = glm::translate(propellerMatrix, glm::vec3(i * 0.55f, 0.1f, j * 0.0f)); 
            propellerMatrix = glm::rotate(propellerMatrix, propellerRotation, glm::vec3(0, 1, 0)); 
            propellerMatrix = glm::scale(propellerMatrix, glm::vec3(0.02f, 0.02f, 0.3f)); 
            RenderMesh2(meshes["blackCube"], shaders["ObjectShader"], propellerMatrix, 2);
        }
    }
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glm::mat4 propellerMatrix = modelMatrix;
            propellerMatrix = glm::translate(propellerMatrix, glm::vec3(i * 0.0f, 0.1f, j * 0.55f));
            propellerMatrix = glm::rotate(propellerMatrix, propellerRotation, glm::vec3(0, 1, 0));
            propellerMatrix = glm::scale(propellerMatrix, glm::vec3(0.02f, 0.02f, 0.3f)); 
            RenderMesh2(meshes["blackCube"], shaders["ObjectShader"], propellerMatrix,2);
        }
    }
}

void Tema2::RenderMesh2(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int objectID = -1)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();

    if (objectID != -1 && shader == shaders["ObjectShader"]) {
        glUniform1i(glGetUniformLocation(shader->GetProgramID(), "objectID"), objectID);
    }

    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderMeshOrtho(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int objectID = -1)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();

    if (objectID != -1 && shader == shaders["ObjectShader"]) {
        glUniform1i(glGetUniformLocation(shader->GetProgramID(), "objectID"), objectID);
    }

    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(minimapCamera->GetViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(minimapProjectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}



void Tema2::OnKeyPress(int key, int mods) {}

void Tema2::OnKeyRelease(int key, int mods) {}


std::vector<glm::vec3> Tema2::GenerateRandomPositions(int count, float terrainSize, float minDistance, const std::vector<glm::vec3>& excludePositions) {
    std::vector<glm::vec3> positions;

    while (positions.size() < count) {
        float x = -terrainSize / 2 + static_cast<float>(rand()) / (RAND_MAX / terrainSize);
        float z = -terrainSize / 2 + static_cast<float>(rand()) / (RAND_MAX / terrainSize);
        glm::vec3 newPosition = glm::vec3(x, 0, z);

        bool valid = true;

        for (const auto& pos : positions) {
            if (glm::distance(pos, newPosition) < minDistance) {
                valid = false;
                break;
            }
        }

        for (const auto& excludePos : excludePositions) {
            if (glm::distance(excludePos, newPosition) < minDistance) {
                valid = false;
                break;
            }
        }

        for (const auto& buildingPos : buildingPositions) {
            glm::vec3 buildingSize = glm::vec3(2.0f, 7.5f, 2.0f);
            AABB buildingBox = getAABB(buildingPos, buildingSize);
            AABB newPosBox = getAABB(newPosition, glm::vec3(minDistance, 0.5f, minDistance));
            if (checkAABBCollision(newPosBox, buildingBox)) {
                valid = false;
                break;
            }
        }

        for (const auto& treePos : treePositions) {
            glm::vec3 trunkSize = glm::vec3(0.5f, 2.5f, 0.5f);
            glm::vec3 leavesSize = glm::vec3(2.0f, 3.0f, 2.0f);
            glm::vec3 leavesPos = treePos + glm::vec3(0, trunkSize.y + leavesSize.y / 2, 0);

            AABB trunkBox = getAABB(treePos, trunkSize);
            AABB leavesBox = getAABB(leavesPos, leavesSize);
            AABB newPosBox = getAABB(newPosition, glm::vec3(minDistance, 0.5f, minDistance));

            if (checkAABBCollision(newPosBox, trunkBox) || checkAABBCollision(newPosBox, leavesBox)) {
                valid = false;
                break;
            }
        }

        if (valid) {
            positions.push_back(newPosition);
        }
    }

    return positions;
}


Tema2::AABB Tema2::getAABB(const glm::vec3& position, const glm::vec3& size) {
    return {
        position - size * 0.5f,
        position + size * 0.5f
    };
}

bool Tema2::checkAABBCollision(const AABB& a, const AABB& b) {
    return (
        a.min.x <= b.max.x && a.max.x >= b.min.x &&
        a.min.y <= b.max.y && a.max.y >= b.min.y &&
        a.min.z <= b.max.z && a.max.z >= b.min.z
        );
}

void Tema2::OnInputUpdate(float deltaTime, int mods) {
    float speed = 7.0f;
    glm::vec3 previousDronePosition = dronePosition;

    glm::vec3 horizontalForward = glm::normalize(glm::vec3(camera->GetForwardVector().x, 0.0f, camera->GetForwardVector().z));

    glm::vec3 movement = glm::vec3(0);
    if (window->KeyHold(GLFW_KEY_W)) movement -= glm::vec3(0, 0, 1) * deltaTime * speed;
    if (window->KeyHold(GLFW_KEY_S)) movement += glm::vec3(0, 0, 1) * deltaTime * speed;
    if (window->KeyHold(GLFW_KEY_A)) movement -= glm::cross(horizontalForward, camera->GetUpVector()) * deltaTime * speed;
    if (window->KeyHold(GLFW_KEY_D)) movement += glm::cross(horizontalForward, camera->GetUpVector()) * deltaTime * speed;
    if (window->KeyHold(GLFW_KEY_Q)) movement -= glm::vec3(0, 1, 0) * deltaTime * speed;
    if (window->KeyHold(GLFW_KEY_E)) movement += glm::vec3(0, 1, 0) * deltaTime * speed;

    glm::vec3 newDronePosition = dronePosition + movement;

    glm::vec3 droneSize = glm::vec3(1.0f, 0.5f, 1.0f);
    AABB droneBox = getAABB(newDronePosition, droneSize);

    bool collisionDetected = false;

    for (const auto& buildingPos : buildingPositions) {
        glm::vec3 buildingSize = glm::vec3(2.0f, 7.5f, 2.0f);
        AABB buildingBox = getAABB(buildingPos, buildingSize);

        glm::vec3 roofSize = glm::vec3(2.0f, 1.0f, 2.0f);
        glm::vec3 roofPos = buildingPos + glm::vec3(0, buildingSize.y / 2 + roofSize.y / 2, 0);
        AABB roofBox = getAABB(roofPos, roofSize);

        if (checkAABBCollision(droneBox, buildingBox) || checkAABBCollision(droneBox, roofBox)) {
            collisionDetected = true;
            break;
        }
    }

    if (!collisionDetected) {
        for (const auto& treePos : treePositions) {
            glm::vec3 trunkSize = glm::vec3(0.5f, 2.5f, 0.5f);
            glm::vec3 leavesSize = glm::vec3(2.0f, 3.0f, 2.0f);
            glm::vec3 leavesPos = treePos + glm::vec3(0, trunkSize.y + leavesSize.y / 2, 0);

            AABB trunkBox = getAABB(treePos, trunkSize);
            AABB leavesBox = getAABB(leavesPos, leavesSize);

            if (checkAABBCollision(droneBox, trunkBox) || checkAABBCollision(droneBox, leavesBox)) {
                collisionDetected = true;
                break;
            }
        }
    }

    if (!collisionDetected) {
        float frequency = 0.05f;
        float amplitude = 0.5f;

        float droneX = newDronePosition.x;
        float droneZ = newDronePosition.z;

        float localX = (newDronePosition.x + 50) / 2.0f;
        float localZ = (newDronePosition.z + 50) / 2.0f;


        float heightAtDronePosition = noise(glm::vec2(localX, localZ) * frequency) * amplitude;

        heightAtDronePosition *= 1.0f;

        if (newDronePosition.y < heightAtDronePosition) {
            collisionDetected = true;
            newDronePosition.y = heightAtDronePosition;
        }
    }

    if (!collisionDetected) {
        dronePosition = newDronePosition;
        camera->TranslateForward(glm::dot(movement, horizontalForward));
        camera->TranslateRight(glm::dot(movement, glm::normalize(glm::cross(horizontalForward, camera->GetUpVector()))));
        camera->TranslateUpward(glm::dot(movement, camera->GetUpVector()));
    }

    float rotationSpeed = glm::radians(60.0f);
    if (window->KeyHold(GLFW_KEY_Z)) droneRotation += rotationSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_X)) droneRotation -= rotationSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_C)) camera->RotateFirstPerson_OY(rotationSpeed * deltaTime);
    if (window->KeyHold(GLFW_KEY_V)) camera->RotateFirstPerson_OY(-rotationSpeed * deltaTime);
}


float Tema2 ::fract(float value) {
    return value - floor(value);
}
float Tema2:: noise(glm::vec2 pos) {
    return fract(sin(glm::dot(pos, glm::vec2(12.9898, 78.233))) * 43758.5453123);
}

