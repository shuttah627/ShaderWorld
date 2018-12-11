#include "Week9Application.h"

#include "stdafx.h"
#include "Surface.h"

void Surface::init()
{
	cameraPosition[0] = 0.0f;
	cameraPosition[1] = 3.0f;
	cameraPosition[2] = -6.0f;
	cameraForward[0] = 0.0f;
	cameraForward[1] = 0.0f;
	cameraForward[2] = -1.0f;
	cameraUp[0] = 0.0f;
	cameraUp[1] = 1.0f;
	cameraUp[2] = 0.0f;
	speed = 0.2f;
	angleSpeed = 0.3f;
}

LRESULT CALLBACK TWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

Surface::Surface(HINSTANCE hInst, HWND hWnd)
{
	init();

	WNDCLASSEX winClass;
	// populate window class struct
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = 0;                                     // class styles: CS_OWNDC, CS_PARENTDC, CS_CLASSDC, CS_GLOBALCLASS, ...
	winClass.lpfnWndProc = TWndProc;                  // pointer to window procedure
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInst;                              // owner of this class
	winClass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WEEK9APPLICATION));   // default icon
	winClass.hIconSm = 0;
	winClass.hCursor = LoadCursor(0, IDC_ARROW);              // default arrow cursor
	winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);   // default white brush
	winClass.lpszMenuName = 0;
	winClass.lpszClassName = L"GraphicsWindow";
	winClass.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));   // default small icon
	RegisterClassEx(&winClass);

	RECT rect;
	GetClientRect(hWnd, &rect);
	handle = CreateWindowEx(WS_EX_CLIENTEDGE, L"GraphicsWindow", L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, rect.right, rect.bottom, hWnd, NULL, hInst, NULL);

	DWORD x = GetLastError();

	hDC = GetDC(handle);

	PIXELFORMATDESCRIPTOR pfd, *ppfd;
	int pixelformat;

	ppfd = &pfd;

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->nVersion = 1;
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ppfd->dwLayerMask = PFD_MAIN_PLANE;
	ppfd->iPixelType = PFD_TYPE_RGBA;
	ppfd->cColorBits = 32;
	ppfd->cDepthBits = 24;	// Was 16 bits
	ppfd->cAccumBits = 0;
	ppfd->cStencilBits = 0;

	pixelformat = ChoosePixelFormat(hDC, ppfd);
	SetPixelFormat(hDC, pixelformat, ppfd);

	HGLRC hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	GetClientRect(handle, &rect);
	windowWidth = rect.right - rect.left;
	windowHeight = rect.bottom - rect.top;

	resizeGL(rect.right - rect.left, rect.bottom - rect.top);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		exit(0);
	}

	initializeGL();

	ShowWindow(handle, SW_SHOWDEFAULT);
}


Surface::~Surface(void)
{
}

void Surface::resize(int width, int height)
{
	MoveWindow(handle, 0, 0, width, height, TRUE);
	windowWidth = width;
	windowHeight = height;
	resizeGL(width, height);
}

void Surface::initializeGL()

{
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 0, 0, 1);

	world = new World();
}

void Surface::resizeGL(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height);
}

void Surface::paintGL()

{
	glClearColor(0.2f, 0.2f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view = mat4(1.0f);
	mat4 proj = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 100.0f);
	mat4 shadow = mat4(1.0f);

	vec3 dirforward(cameraForward[0], cameraForward[1], cameraForward[2]);
	glm::normalize(dirforward);
	vec3 dirup(cameraUp[0], cameraUp[1], cameraUp[2]);
	glm::normalize(dirup);
	vec3 dirside = glm::cross(dirup, dirforward);

	mat4 rot = mat4(dirside[0], dirside[1], dirside[2], 0.0,
		dirup[0], dirup[1], dirup[2], 0.0,
		dirforward[0], dirforward[1], dirforward[2], 0.0,
		0.0, 0.0, 0.0, 1.0);
	mat4 M = glm::translate(mat4(1.0f), vec3(cameraPosition[0], cameraPosition[1], cameraPosition[2])) * rot;
	view = glm::inverse(M);

	world->setWindowSize(windowWidth, windowHeight);
	world->render(view, proj, shadow);
	SwapBuffers(hDC);
}

void Surface::strafe(float direction)
{
	vec3 dirforward(cameraForward[0], cameraForward[1], cameraForward[2]);
	glm::normalize(dirforward);
	vec3 dirup(cameraUp[0], cameraUp[1], cameraUp[2]);
	glm::normalize(dirup);
	vec3 dirside = glm::cross(dirup, dirforward);

	cameraPosition[0] += speed * direction * dirside.x;
	cameraPosition[1] += speed * direction * dirside.y;
	cameraPosition[2] += speed * direction * dirside.z;
}

void Surface::step(float direction)
{
	vec3 dirforward(cameraForward[0], cameraForward[1], cameraForward[2]);
	glm::normalize(dirforward);

	cameraPosition[0] += speed * direction * dirforward.x;
	cameraPosition[1] += speed * direction * dirforward.y;
	cameraPosition[2] += speed * direction * dirforward.z;
}

mat3 Surface::rotationVectorAngle(float ux, float uy, float uz, float angle)

{
	// http://en.wikipedia.org/wiki/Rotation_matrix
	float ct = cos(angle);
	float st = sin(angle);
	float oct = 1.0f - ct;
	float ost = 1.0f - st;
	mat3 rot = mat3(ct + ux * ux * oct, ux * uy * oct - uz * st, ux * uz * oct + uy * st,
		uy * ux * oct + uz * st, ct + uy * uy * oct, uy * uz * oct - ux * st,
		uz * ux * oct - uy * st, uz * uy * oct + ux * st, ct + uz * uz * oct);
	return rot;
}

void Surface::turnside(float direction)

{
	vec3 dirforward(cameraForward[0], cameraForward[1], cameraForward[2]);
	glm::normalize(dirforward);
	vec3 dirup(cameraUp[0], cameraUp[1], cameraUp[2]);
	glm::normalize(dirup);

	float angle = direction * angleSpeed;
	mat3 R = rotationVectorAngle(dirup.x, dirup.y, dirup.z, angle);

	dirforward = R * dirforward;

	cameraForward[0] = dirforward.x;
	cameraForward[1] = dirforward.y;
	cameraForward[2] = dirforward.z;
}

void Surface::turnup(float direction)

{
	vec3 dirforward(cameraForward[0], cameraForward[1], cameraForward[2]);
	glm::normalize(dirforward);
	vec3 dirup(cameraUp[0], cameraUp[1], cameraUp[2]);
	glm::normalize(dirup);
	vec3 dirside = glm::cross(dirup, dirforward);

	float angle = direction * angleSpeed;
	mat3 R = rotationVectorAngle(dirside.x, dirside.y, dirside.z, angle);

	dirforward = R * dirforward;
	dirup = R * dirup;

	cameraForward[0] = dirforward.x;
	cameraForward[1] = dirforward.y;
	cameraForward[2] = dirforward.z;
	cameraUp[0] = dirup.x;
	cameraUp[1] = dirup.y;
	cameraUp[2] = dirup.z;
}
