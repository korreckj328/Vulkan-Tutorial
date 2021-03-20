#pragma once

#include "GraphicsPipeline.h"
#include "ObjectBuffers.h"
#include "Descriptor.h"
#include "Camera.h"

class ObjectRenderer {
public:
	ObjectRenderer();
	~ObjectRenderer();
	void createObjectRenderer(MeshType modelType, glm::vec3 _position, glm::vec3 _scale);
	void updateUniformBuffer(Camera camera);
	void draw();
	void destroy();
private:
	GraphicsPipeline gPipeline;
	ObjectBuffers objBuffers;
	Descriptor descriptor;
	glm::vec3 position;
	glm::vec3 scale;
};
