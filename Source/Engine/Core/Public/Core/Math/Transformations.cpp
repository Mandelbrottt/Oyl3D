#include "Transformations.h"

#include "Matrix3.h"
#include "Scalar.h"
#include "Vector4.h"

namespace Oyl::Matrix
{
	Matrix4
	Perspective(float a_fieldOfView, float a_aspectRatio, float a_nearPlane, float a_farPlane)
	{
		Matrix4 result { 0 };

		float tanFov = std::tan(a_fieldOfView * 0.5f * Math::DEG_TO_RAD);

		result[0][0] = 1.0f / (a_aspectRatio * tanFov);
		result[1][1] = -1.0f / (tanFov);
		result[2][2] = -(a_farPlane + a_nearPlane) / (a_farPlane - a_nearPlane);
		result[2][3] = -1.0f;
		result[3][2] = -(2 * a_farPlane * a_nearPlane) / (a_farPlane - a_nearPlane);
		return result;
	}

	Matrix3
	RotateX(float a_angle)
	{
		// std::cos and sin expect radians
		a_angle *= Math::DEG_TO_RAD;

		Matrix3 result;

		result[0][0] = 1.0f;
		result[1][1] =  std::cos(a_angle);
		result[1][2] =  std::sin(a_angle);
		result[2][1] = -std::sin(a_angle);
		result[2][2] =  std::cos(a_angle);

		return result;
	}

	Matrix3
	RotateY(float a_angle)
	{
		// std::cos and sin expect radians
		a_angle *= Math::DEG_TO_RAD;

		Matrix3 result;

		result[0][0] =  std::cos(a_angle);
		result[0][2] =  std::sin(a_angle);
		result[1][1] = 1.0f;
		result[2][0] = -std::sin(a_angle);
		result[2][2] =  std::cos(a_angle);

		return result;
	}

	Matrix3
	RotateZ(float a_angle)
	{
		// std::cos and sin expect radians
		a_angle *= Math::DEG_TO_RAD;

		Matrix3 result;

		result[0][0] =  std::cos(a_angle);
		result[0][1] =  std::sin(a_angle);
		result[1][0] = -std::sin(a_angle);
		result[1][1] =  std::cos(a_angle);
		result[2][2] = 1.0f;

		return result;
	}

	Matrix3
	Rotate(float a_angle, Vector3 a_axis)
	{
		Vector3 u = Vector::Normalize(a_axis);

		float sin_theta = std::sin(a_angle * Math::DEG_TO_RAD);
		float cos_theta = std::cos(a_angle * Math::DEG_TO_RAD);

		float one_minus_cos_theta = 1 - cos_theta;

		Matrix3 result;

		result[0][0] = cos_theta + Math::Square(u.x) * one_minus_cos_theta;
		result[0][1] = u.y * u.x * one_minus_cos_theta + u.z * sin_theta;
		result[0][2] = u.z * u.x * one_minus_cos_theta - u.y * sin_theta;

		result[1][0] = u.x * u.y * one_minus_cos_theta - u.z * sin_theta;
		result[1][1] = cos_theta + Math::Square(u.y) * one_minus_cos_theta;
		result[1][2] = u.z * u.y * one_minus_cos_theta + u.x * sin_theta;

		result[2][0] = u.x * u.z * one_minus_cos_theta + u.y * sin_theta;
		result[2][1] = u.y * u.z * one_minus_cos_theta - u.x * sin_theta;
		result[2][2] = cos_theta + Math::Square(u.z) * one_minus_cos_theta;

		return result;
	}

	Matrix4
	Scale(Vector3 a_scale)
	{
		Matrix4 result;

		result[0][0] = a_scale.x;
		result[1][1] = a_scale.y;
		result[2][2] = a_scale.z;
		result[3][3] = 1.0f;

		return result;
	}

	Matrix4
	Translate(Vector3 a_position)
	{
		Matrix4 result = Matrix4::Identity();

		result[3][0] = a_position.x;
		result[3][1] = a_position.y;
		result[3][2] = a_position.z;

		return result;
	}

	Matrix4
	ViewInverse(Matrix4 a_locRot)
	{
		Matrix4 result { 0 };

		Matrix3 rotation = {
			Vector3(a_locRot[0]),
			Vector3(a_locRot[1]),
			Vector3(a_locRot[2]),
		};

		rotation.Transpose();

		// Negate the position vector
		Vector3 position = Vector3(a_locRot[3]);
		position = -(position * rotation);

		result[0] = Vector4(rotation[0], 0);
		result[1] = Vector4(rotation[1], 0);
		result[2] = Vector4(rotation[2], 0);
		result[3] = Vector4(position, 1.0f);

		return result;
	}

	// Inspired by GLM lookAt_RH()
	Matrix4
	LookAt(Vector3 a_position, Vector3 a_target, Vector3 a_up)
	{
		Vector3 f = Vector::Normalize(a_target - a_position);
		Vector3 s = Vector::Normalize(Vector::Cross(f, a_up));
		Vector3 u = Vector::Cross(s, f);

		Matrix4 result = Matrix4::Identity();
		result[0][0] = s.x;
		result[1][0] = s.y;
		result[2][0] = s.z;
		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[0][2] = -f.x;
		result[1][2] = -f.y;
		result[2][2] = -f.z;
		result[3][0] = -Vector::Dot(s, a_position);
		result[3][1] = -Vector::Dot(u, a_position);
		result[3][2] = Vector::Dot(f, a_position);
		return result;
	}

	// https://gamedev.stackexchange.com/a/50968
	Vector3
	EulerAngles(Matrix3 a_matrix)
	{
		Vector3 result;

		float& yaw   = result.y;
		float& pitch = result.x;
		float& roll  = result.z;

		if (a_matrix[0][0] == 1.0f || a_matrix[0][0] == -1.0f)
		{
			yaw = Math::Atan2(a_matrix[0][2], a_matrix[2][3]);
			pitch = 0;
			roll = 0;
		} else
		{
			yaw = Math::Atan2(-a_matrix[2][0], a_matrix[0][0]);
			pitch = Math::Asin(1 / a_matrix[1][0]);
			roll = Math::Atan2(-a_matrix[1][2], a_matrix[1][1]);
		}

		return result;
	}
}
