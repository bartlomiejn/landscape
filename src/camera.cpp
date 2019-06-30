#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>

// Public

Camera::Camera(
	glm::vec3 position, glm::vec3 world_up, float yaw, float pitch,
	float fov
) :
	cam_front(glm::vec3(0.0f, 0.0f, -1.0f)),
	cam_position(position),
	cam_world_up(world_up),
	cam_yaw(yaw),
	cam_pitch(pitch),
	cam_fov(fov)
{
	update_camera_vectors();
}

void
Camera::move(Direction direction, float delta)
{
	switch (direction)
	{
		case direction_forward:
			cam_position += cam_front * delta;
			break;
		case direction_backward:
			cam_position -= cam_front * delta;
			break;
		case direction_left:
			cam_position -= cam_right * delta;
			break;
		case direction_right:
			cam_position += cam_right * delta;
			break;
	}
}

void
Camera::rotate(float x_offset, float y_offset)
{
	cam_yaw = glm::mod(cam_yaw + x_offset, 360.0f);
	cam_pitch -= y_offset;
	
	if (cam_pitch > 89.0f)
		cam_pitch = 89.0f;
	if (cam_pitch > -89.0f)
		cam_pitch = -89.0f;
	
	update_camera_vectors();
}

void
Camera::zoom_in(float delta)
{
	if (cam_fov >= 1.0f && cam_fov <= 45.0f)
		cam_fov -= delta;
	if (cam_fov <= 1.0f)
		cam_fov = 1.0f;
	if (cam_fov >= 45.0f)
		cam_fov = 45.0f;
}

const glm::vec3&
Camera::position()
{
	return cam_position;
}

const glm::vec3&
Camera::front()
{
	return cam_front;
}

const glm::vec3&
Camera::up()
{
	return cam_up;
}

const glm::vec3&
Camera::right()
{
	return cam_right;
}

const glm::vec3&
Camera::world_up()
{
	return cam_world_up;
}

const float
Camera::yaw()
{
	return cam_yaw;
}

const float
Camera::pitch()
{
	return cam_pitch;
}

const float
Camera::mvmt_speed()
{
	return cam_mvmt_speed;
}

const float
Camera::mouse_sensitivity()
{
	return cam_mouse_sensitivity;
}

const float
Camera::fov()
{
	return cam_fov;
}

glm::mat4
Camera::view_matrix()
{
	return glm::lookAt(cam_position, cam_position + cam_front, cam_up);
}

// Private

void
Camera::update_camera_vectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(cam_pitch)) * cos(glm::radians(cam_yaw));
	front.y = sin(glm::radians(cam_pitch));
	front.z = cos(glm::radians(cam_pitch)) * sin(glm::radians(cam_yaw));
	cam_front = glm::normalize(front);
	cam_right = glm::normalize(glm::cross(cam_front, cam_world_up));
	cam_up = glm::normalize(glm::cross(cam_right, cam_front));
}