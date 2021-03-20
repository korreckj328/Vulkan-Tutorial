#include "Camera.h"

Camera::Camera() {

}

Camera::~Camera() {

}

void Camera::init(float FOV, float width, float height, float nearPlane, float farPlane) {
	cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::perspective(FOV, width / height, nearPlane, farPlane);
	viewMatrix = glm::lookAt(cameraPos, cameraFront, cameraUp);
}

glm::mat4 Camera::getViewMatrix() {
	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
	return projectionMatrix;
}

void Camera::setCameraPosition(glm::vec3 position) {
	cameraPos = position;
}


