#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


#include <D3DX10math.h>


class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetLookAt(float, float , float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotaion();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_lookAtX, m_lookAtY, m_lookAtZ;
	D3DXMATRIX m_viewMatrix;
};

#endif