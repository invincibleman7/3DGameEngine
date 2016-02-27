#include "Cameraclass.h"


CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_lookAtX = 0.0f;
	m_lookAtY = 0.0f;
	m_lookAtZ = 1.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}

void CameraClass::SetLookAt(float x, float y, float z)
{
	m_lookAtX = x;
	m_lookAtY = y;
	m_lookAtZ = z;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}

D3DXVECTOR3 CameraClass::GetRotaion()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// setup the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// setup the position of the camera in the world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// setup where the camero is looking by default
	lookAt.x = m_lookAtX;
	lookAt.y = m_lookAtY;
	lookAt.z = m_lookAtZ;

	// set the yaw(Y axis), pitch(X axis), and roll(Z axis) rotations in radians
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// create teh rotaion matrix from the yaw, pitch and roll values
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// transform the lookat and up vector by the rotation matrix so the view is correctly rotated at the origin
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// transform the rotated camera position to the location of the viewer
	lookAt = position + lookAt;

	// finally create the view matrix from the three updated vectors
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);


	return;
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
