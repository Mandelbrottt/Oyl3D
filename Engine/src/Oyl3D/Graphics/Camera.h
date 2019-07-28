#pragma once

namespace oyl {

// TODO: have a dirty flag

class OrthographicCamera {
public:
	OrthographicCamera(float left, float right, float bottom, float top);

	inline const glm::mat4& getViewMatrix() const { return m_view; }
	inline const glm::mat4& getProjectionMatrix() const { return m_projection; }
	inline const glm::mat4& getViewProjectionMatrix() const { return m_viewProjection; }
 
	inline const glm::vec3& getPosition() const { return m_position; };
	inline void setPosition(const glm::vec3& position) { m_position = position; recalculateViewMatrix(); }
	
	inline const float getRotation() const { return m_rotation; }
	inline void setRotation(float rotation) { m_rotation = rotation; recalculateViewMatrix(); }
private:
	void recalculateViewMatrix();
private:
	glm::mat4 m_view;
	glm::mat4 m_projection;
	glm::mat4 m_viewProjection;

	glm::vec3 m_position = glm::vec3(0);
	float m_rotation = 0.0f;
};

// TODO: Swap to quaternion rotation
class PerspectiveCamera {
public:
	PerspectiveCamera(float fov, float aspect, float nearZ, float farZ);

	inline const glm::mat4& getViewMatrix() const { return m_view; }
	inline const glm::mat4& getProjectionMatrix() const { return m_projection; }
	inline const glm::mat4& getViewProjectionMatrix() const { return m_viewProjection; }

	inline const glm::vec3& getPosition() const { return m_position; };
	inline void setPosition(glm::vec3& position) { m_position = position; recalculateViewMatrix(); }

	inline const glm::vec3 getRotation() const { return m_rotation; }
	inline void setRotation(glm::vec3& rotation) { m_rotation = rotation; recalculateViewMatrix(); }

	inline void setFOV(float fov) { m_fov = fov; recalculateProjMatrix(); }
	inline void setAspect(float aspect) { m_aspect = aspect; recalculateProjMatrix(); }
	inline void setNearZ(float nearZ) { m_nearZ = nearZ; recalculateProjMatrix(); }
	inline void setFarZ(float farZ) { m_farZ = farZ; recalculateProjMatrix(); }
private:
	void recalculateViewMatrix();
	void recalculateProjMatrix();
private:
	float m_fov;
	float m_aspect;
	float m_nearZ;
	float m_farZ;

	glm::mat4 m_view;
	glm::mat4 m_projection;
	glm::mat4 m_viewProjection;

	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_rotation = glm::vec3(0);

};

}
