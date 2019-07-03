#ifndef LANDSCAPE_CAMERA_H
#define LANDSCAPE_CAMERA_H

#include <glm/glm.hpp>

enum Direction
{
	direction_forward,
	direction_backward,
	direction_left,
	direction_right
};

class Camera
{
public:
	Camera(
		glm::vec3 position, glm::vec3 world_up, float yaw, float pitch,
		float fov);
	void move(Direction direction, float length);
	void rotate(float x_offset, float y_offset);
	void zoom_in(float delta);
	const glm::vec3 &position();
	const glm::vec3 &front();
	const glm::vec3 &up();
	const glm::vec3 &right();
	const glm::vec3 &world_up();
	const float yaw();
	const float pitch();
	const float mvmt_speed();
	const float mouse_sensitivity();
	const float fov();
	glm::mat4 view_matrix();
private:
	glm::vec3 cam_position;
	glm::vec3 cam_front;
	glm::vec3 cam_up;
	glm::vec3 cam_right;
	glm::vec3 cam_world_up;
	float cam_yaw;
	float cam_pitch;
	float cam_mvmt_speed;
	float cam_mouse_sensitivity;
	float cam_fov;
	void update_camera_vectors();
};

#endif //LANDSCAPE_CAMERA_H
