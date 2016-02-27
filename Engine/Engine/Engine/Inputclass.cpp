#include "Inputclass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;
	int i;

	// initialize all the keys to being released and not pressed
	for (i=0;i<256;i++)
	{
		m_keys[i] = false;
	}


	// store the screen size which will be userd for positioning the mouse cursor
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// intialize the location of the mouse on the screen
	m_mouseX = 0;
	m_mouseY = 0;
	m_wheelZ = 0;


	// initialize the main direct input interface
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}


	// initialize the direct input interface for the keyboard
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// set the data format. In this case since it is a keyboard we can use the predefined data format
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}


	// set the cooperative level of the keyboard to not share with other programs
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// now acquire the keyboard
	result = m_keyboard->Acquire();
	if(result)
	{
		return false;
	}



	// initialize the direct input interface for the mouse
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// set the data format for the mouse using the pre-defined mouse data format
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// set the cooperative level of the mouse to share with other programs
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// acquire the mouse
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	// release the mouse
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// release the keyboard
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// release the main interface to direct input
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;


	// read the current state of the keyboard
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// read the current state of the mouse
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// process the changes in the mouse and keyboard
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;


	// read the keyboard device
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// if the keyboard lost focus or was not acquired then try to get control back
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}


	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;


	// read the mouse device
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// if the mouse lost focus or was not acquired then try to get control back
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}


	return true;
}

void InputClass::ProcessInput()
{
	// update the location of the mouse cursor based on the change of the mouse location during the frame
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;
	m_wheelZ += m_mouseState.lZ;

	// ensure the mouse location doesn't exceed the screen width or height
	if(m_mouseX < 0) { m_mouseX = 0;}
	if(m_mouseY < 0) { m_mouseY = 0;}

	if(m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }


	return;
}

bool InputClass::IsEscapePressed()
{
	// do a bitwise and on the keyboard state to check if the escape key is currently being pressed
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftMouseButtonPressed()
{
	return m_mouseState.rgbButtons[0];
}

bool InputClass::IsRightMouseButtonPressed()
{
	return m_mouseState.rgbButtons[1];
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY, int& wheelZ)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	wheelZ = m_wheelZ;
	return;
}