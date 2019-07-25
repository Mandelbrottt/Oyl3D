#include "oylpch.h"
#include "Camera.h"

namespace oyl {

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	: m_projection(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_view(1.0f) {
	m_viewProjection = m_projection * m_view;
}

void OrthographicCamera::recalculateViewMatrix() {
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position);

	transform = glm::rotate(transform, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

	m_view = glm::inverse(transform);
	m_viewProjection = m_projection * m_view;
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearZ, float farZ)
	: m_projection(glm::perspective(glm::radians(fov), aspect, nearZ, farZ)), m_view(1.0f) {
	m_viewProjection = m_projection * m_view;
}

void PerspectiveCamera::recalculateViewMatrix(){
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position);

	transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Z
	transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Y
	transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // X

	m_view = inverse(transform);
	m_viewProjection = m_projection * m_view;
}

}



