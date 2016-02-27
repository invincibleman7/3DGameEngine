#include "Graphicsclass.h"
#include <string>

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// create the direct3d object
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// initialize the direct3d object
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Count not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// create the camera object
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// set the initial position of the camera
	m_Camera->SetPosition(0.0f, 1.0f, -5.0f);

	// create the model object
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// initialize the model object
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/model.txt", L"../Engine/data/girl.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initilize the model object", L"Error", MB_OK);
		return false;
	}

	// create the light shader object
	m_LightShader = new ShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// initialize the light shader object
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);
		return false;
	}


	// create the light object
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// initialize the light object
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.3f, -1.0f, 1.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	//////////////////////////////////////////////////////////////////////////
	//test//
	// show the graphics card infomation
	/*char cardName[512];
	char cardMemStr[20];
	int cardMemory;
	m_D3D->GetVideoCardInfo(cardName, cardMemory);
	_itoa_s(cardMemory, cardMemStr, 10);

	strcat_s(cardName, "\ncard memory: ");
	strcat_s(cardMemStr, "M");
	std::string name = cardName;
	std::string mem = cardMemStr;
	name += mem;

	std::wstring stemp = std::wstring(name.begin(), name.end());
	LPCWSTR sw = stemp.c_str();

	MessageBox(NULL, sw, L"", MB_OK);*/
	//////////////////////////////////////////////////////////////////////////

	// release the light object
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// release the light shader object
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// release the model object
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// release the camera object
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	

	return;
}


bool GraphicsClass::Frame(int mouseX, int mouseY, int wheelZ, int mousePressState)
{
	bool result;
	float camRotH, camRotV, camDistIncre;

	static float rotation = 0.0f;
	static int lastMouseY = -1;
	static int lastMouseX = -1;
	static int lastWheelZ = 0;

	// 0 : left mouse button pressed
	// 1 : Right mouse button pressed

	if(mousePressState != 1) // pressing right mouse button
	{
		lastMouseX = -1;
		lastMouseY = -1;
	}

	camRotH = (lastMouseX == -1) ? 0 : (mouseX - lastMouseX);
	camRotV = (lastMouseY == -1) ? 0 : (mouseY - lastMouseY);
	camDistIncre = wheelZ - lastWheelZ;

	// update the rotation variable each frame
	/*rotation += (float)D3DX_PI * 0.01f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}*/

	// render the graphics scene
	result = Render(camRotH, camRotV, camDistIncre);
	if(!result)
	{
		return false;
	}


	lastMouseX = mouseX;
	lastMouseY = mouseY;
	lastWheelZ = wheelZ;

	return true;
}


bool GraphicsClass::Render(float camRotH, float camRotV, float cameraDistIncre)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;


	// clear the buffers to begin the scene
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);






	//////////////////////////////////////////////////////////////////////////
	// Rotate and scale viewport according to mouse position changing
	// very ugly method *_*
	// generate the view matrix based on the camera's position
	D3DXVECTOR3 cameraPos = m_Camera->GetPosition();
	D3DXVECTOR3 targetPoint = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 increment = targetPoint - cameraPos;
	float wheelSensitive = 0.001f;
	cameraPos.x += increment.x * (float)cameraDistIncre * wheelSensitive;
	cameraPos.y += increment.y * (float)cameraDistIncre * wheelSensitive;
	cameraPos.z += increment.z * (float)cameraDistIncre * wheelSensitive;

	// rotate horizontal
	D3DXMATRIX camRotMatrix;
	D3DXMatrixIdentity(&camRotMatrix);
	D3DXMatrixRotationY(&camRotMatrix, camRotH * 0.01f);
	D3DXVECTOR4 tempVec;
	D3DXVec3Transform(&tempVec, &cameraPos, &camRotMatrix);
	cameraPos = { tempVec.x, tempVec.y, tempVec.z };

	// rotate vertical
	D3DXVECTOR3 VRotAxis;
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 mToCameraVec = targetPoint - cameraPos;
	D3DXVec3Cross(&VRotAxis, &mToCameraVec, &up);
	D3DXMatrixIdentity(&camRotMatrix);
	D3DXMatrixRotationAxis(&camRotMatrix, &VRotAxis, -camRotV * 0.01f);
	D3DXVec3Transform(&tempVec, &cameraPos, &camRotMatrix);
	cameraPos = { tempVec.x, tempVec.y, tempVec.z };

	m_Camera->SetPosition(cameraPos.x, cameraPos.y, cameraPos.z);
	m_Camera->SetLookAt(targetPoint.x - cameraPos.x, targetPoint.y - cameraPos.y, targetPoint.z - cameraPos.z);
	//////////////////////////////////////////////////////////////////////////











	m_Camera->Render();

	// get the world, view, and projection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);


	// rotate the world matrix by the rotaion value so that the triangle will spin
	//D3DXMatrixRotationY(&worldMatrix, rotation);
	/*D3DXVECTOR3 axis = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	D3DXMatrixRotationAxis(&worldMatrix, &axis, rotation);*/


	// put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_D3D->GetDeviceContext());

	// render the model using the light shader
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix ,viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}



	// present the rendered scene to the screen
	m_D3D->EndScene();


	return true;
}