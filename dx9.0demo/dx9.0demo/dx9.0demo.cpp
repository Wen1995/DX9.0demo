//dx9.0demo.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "dx9.0demo.h"
#include <mmsystem.h>

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//交互相关

//---------------------------------------------------
HWND hwnd;
IDirect3DDevice9 *iDevice = 0;
IDirect3DVertexBuffer9 *iVB = 0;
IDirect3DIndexBuffer9 *iIB = 0;
IDirect3DTexture9 *iTex = 0;
D3DMATERIAL9 Mtrl;
D3DMATERIAL9 TeapotMtrl;
D3DXMATRIX TeapotPos;
D3DMATERIAL9 MirrorMtrl;
IDirect3DTexture9 *Texture;
float TIMER = 0.0f;
float Toward = 1.0f;
//---------------------------------------------------
bool Setup();
bool Cleanup();
bool Display();
bool Display(float timeDelta);
void EnterMsgLoop(HACCEL hAccelTable, bool (*ptr_display)(float deltaTime));

void D3DInit();

//创建资源
void DrawIndexedPrimitive();
void DrawCube();
void DrawColoredTriangle();
void DrawTexQuad();
void DrawTransparentTeapot();
void CreateTeapot();
void CreteMirror();
//光源
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);
//d3dx自带几何体
ID3DXMesh* meshes[4];
//
void RenderMirror();	//实现镜面反射效果
void RenderShadow();	//实现阴影效果


//-------------------------------------------------------------------------
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	
    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX90DEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
	
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX90DEMO));

	//初始化DirectX
	D3DInit();
	//初始化DirectX资源
	Setup();
	//Display();



    // 主消息循环: 
	EnterMsgLoop(hAccelTable, Display);

//MSG msg;
 //   while (GetMessage(&msg, nullptr, 0, 0))
 //   {
 //       if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
 //       {
 //           TranslateMessage(&msg);
 //           DispatchMessage(&msg);
 //       }
 //   }
	Cleanup();
	return 0;
    //return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX90DEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX90DEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   hwnd = hWnd;
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			//Display();
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

	case WM_KEYDOWN:
		break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


bool Setup()
{
	//资源创建 创建材质，索引等， 设置一些绘制状态-----------------
	//DrawIndexedPrimitive();		//点线立方体
	//DrawCube();					//启用光照的材质立方体
	//DrawTexQuad();
	//DrawTransparentTeapot();
	//CreteMirror();
	CreateTeapot();
	//-------------------------光照--------------------------------
	//启用光照
	iDevice->SetRenderState(D3DRS_LIGHTING, true);

	//创建光源
	D3DXVECTOR3 dir = D3DXVECTOR3(0.707f, -0.707f, 0.707f);
	D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 directLight = InitDirectionalLight(&dir, &col);

	iDevice->SetLight(0, &directLight);
	iDevice->LightEnable(0, true);

	//设置其他绘制状态
	iDevice->SetRenderState(D3DRS_NORMALDEGREE, true);		//规范化法向量
	iDevice->SetRenderState(D3DRS_SPECULARENABLE, true);	//启用镜面高光


	//-------------------------------------------------------------


	//设置摄像机
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);	//摄像机的位置
	D3DXVECTOR3 target(0.0f, 0.0f, 1.0f);		//摄像机的朝向
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);			//朝上的向量
	D3DXMATRIX lookAtMatrix;
	D3DXMatrixLookAtLH(&lookAtMatrix, &position, &target, &up);		//获得观察矩阵

	iDevice->SetTransform(D3DTS_VIEW, &lookAtMatrix);				//设置观察矩阵

	D3DXMATRIX proj;			//获得投影矩阵
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f,
		(float)800 / (float)600,
		1.0f,
		1000.0f);
	iDevice->SetTransform(D3DTS_PROJECTION, &proj);		//设置投影矩阵

	//绘制状态


	return true;
}
bool Cleanup()
{
	//TODO
	return true;
}

// 进入消息循环
void EnterMsgLoop(HACCEL hAccelTable, bool(*ptr_display)(float deltaTime))
{
	MSG msg;
	static float lastTime = timeGetTime();
	static float currentTime = 0;
	// 主消息循环: 
	while (1)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			currentTime = timeGetTime();
			ptr_display(currentTime - lastTime);
			lastTime = currentTime;
		}
	}
	//因为GetMessage在消息队列没有消息的时候不会返回，不能执行到display
	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//	currentTime = timeGetTime();
	//	ptr_display(currentTime - lastTime);
	//	lastTime = currentTime;
	//}
}


bool Display(float timeDelta)
{
	////摄像机的旋转
	//if (::GetAsyncKeyState('A') & 0x8000f)
	//{
	//	Toward += 0.001f;
	//	if (Toward > 1.0f)
	//		Toward = 1.0f;
	//}
	//if (::GetAsyncKeyState('D') & 0x8000f)
	//{
	//	Toward -= 0.001f;
	//	if (Toward < 0.0f)
	//		Toward = 0.0f;
	//}

	if (iDevice == nullptr)
	{
		::MessageBox(0, L"Direct未初始化", 0, 0);
		return false;
	}

	//----------------------------变换------------------------
	
	//
	D3DXMATRIX R;

	//几何体不停旋转
	//D3DXMATRIX Rx, Ry;
	//D3DXMatrixRotationX(&Rx, 0);
	//
	//D3DXMatrixRotationY(&Ry, TIMER);
	//TIMER += 0.001;
	//if (TIMER >= 6.28f)
	//	TIMER = 0.0f;
	//D3DXMATRIX worldMatrix = Rx * Ry;
	//iDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

	//在不同位置使用两种着色模式绘制
	

	//--------------------------------------------------------

	//开始绘制
	iDevice->Clear(
		0,
		0,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		0xffffffff, 1.0f,
		0);
	iDevice->BeginScene();
	
	//绘制镜子
	iDevice->SetMaterial(&MirrorMtrl);
	iDevice->SetTexture(0, iTex);
	iDevice->SetStreamSource(0, iVB, 0, sizeof(NormalTexVertex));			//设置输入源
	//iDevice->SetIndices(iIB);												//设置索引
	iDevice->SetFVF(NormalTexVertex::FVF);									//设置顶点格式
	D3DXMatrixIdentity(&R);
	D3DXMatrixTranslation(&R, 5.0f, 0.0f, 5.0f);
	iDevice->SetTransform(D3DTS_WORLD, &R);
//	iDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	
	//绘制茶壶
	iDevice->SetMaterial(&TeapotMtrl);
	iDevice->SetTransform(D3DTS_WORLD, &TeapotPos);
	//iDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	meshes[0]->DrawSubset(0);
	//iDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	//iDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	
	//RenderMirror();		//绘制映像
	RenderShadow();			//绘制阴影

	iDevice->EndScene();
	iDevice->Present(0, 0, 0, 0);	//显示
	return true;
}



bool Display()
{	
	if (iDevice == nullptr)
	{
		::MessageBox(0, L"Direct未初始化", 0, 0);
		return false;
	}
	//旋转几何体
	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, 0);
	D3DXMatrixRotationY(&Ry, 0);

	//世界矩阵
	D3DXMATRIX worldMatrix = Rx * Ry;
	iDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

	//开始绘制
	iDevice->Clear(
		0,
		0,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		0xffffffff, 1.0f,
		0);
	iDevice->BeginScene();

	iDevice->SetStreamSource(0, iVB, 0, sizeof(XYZVertex));		//设置输入源
	iDevice->SetIndices(iIB);									//设置索引
	iDevice->SetFVF(XYZVertex::FVF);							//设置顶点格式

	iDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	iDevice->EndScene();
	iDevice->Present(0, 0, 0, 0);	//显示
	return true;
}

// 初始化Direct3D
// 1. 获取IDirect3D9（用于获取硬件设备信息）指针
// 2. 检查硬件是否支持硬件顶点计算
// 3. 初始化D3DPRESENT_PARAMETERS(创建IDirect3DDevice9的初始化信息)
// 4. 创建IDirect3DDevice
void D3DInit()
{
	iDevice = nullptr;
	// 获取IDirect3D
	IDirect3D9 *d3d9;		//usage 1.设备枚举 2.创建IDirect3DDevice9
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	//检查硬件性能
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		&caps
	);
	int vp = 0;	//是否支持硬件加速的标记
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//填充D3DPRESENT_PARAMETERS
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth =				800;
	d3dpp.BackBufferHeight =			600;
	d3dpp.BackBufferFormat =			D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount =				1;
	d3dpp.MultiSampleType =				D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality =			0;
	d3dpp.SwapEffect =					D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow =				hwnd;
	d3dpp.Windowed =					true;
	d3dpp.EnableAutoDepthStencil =		true;
	d3dpp.AutoDepthStencilFormat =		D3DFMT_D24S8;
	d3dpp.Flags =						0;
	d3dpp.FullScreen_RefreshRateInHz =	D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval =		D3DPRESENT_INTERVAL_IMMEDIATE;

	// 创建IDirect3DDevice
	HRESULT hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		vp,
		&d3dpp,
		&iDevice
	);

	d3d9->Release();
	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateDevice Failed", 0, 0);
		return;
	}
}


//----------------------顶点+索引-------------------------
//创建一个点线立方体
void DrawIndexedPrimitive()		
{
	//创建顶点缓存和索引缓存
	if (iDevice == nullptr)
	{
		::MessageBox(0, L"IDirectDevice is null", 0, 0);
		return;
	}
	iDevice->CreateVertexBuffer(
		8 * sizeof(XYZVertex),
		D3DUSAGE_WRITEONLY,
		XYZVertex::FVF,
		D3DPOOL_MANAGED,
		&iVB,
		0);
	if (iVB == nullptr)
	{
		::MessageBox(0, L"VertexBuffer create failed", 0, 0);
		return;
	}

	iDevice->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&iIB,
		0);
	if (iIB == nullptr)
	{
		::MessageBox(0, L"IndexBuffer create failed", 0, 0);
		return;
	}

	//填充缓存
	XYZVertex *vertice;
	iVB->Lock(0, 0, (void**)&vertice, 0);
	float len = 2;
	vertice[0] = XYZVertex(-len, -len, -len);
	vertice[1] = XYZVertex(-len, len, -len);
	vertice[2] = XYZVertex(len, len, -len);
	vertice[3] = XYZVertex(len, -len, -len);
	vertice[4] = XYZVertex(-len, -len, len);
	vertice[5] = XYZVertex(-len, len, len);
	vertice[6] = XYZVertex(len, len, len);
	vertice[7] = XYZVertex(len, -len, len);
	iVB->Unlock();

	WORD *indices;
	iIB->Lock(0, 0, (void**)&indices, 0);
	int input[36] = {
		0,1,2,0,2,3,	//front side
		4,6,5,4,7,6,	//back side
		4,5,1,4,1,0,	//left side
		3,2,6,3,6,7,	//right side
		1,5,6,1,6,2,	//top
		4,0,3,4,3,7 };	//bottom
	for (int i = 0;i < 36;i++)
		indices[i] = input[i];
	iIB->Unlock();



	//设置绘制状态
	iDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

}

//创建一个有颜色的三角形
void DrawColoredTriangle()
{
	//创建缓存

	//填充缓存

	//绘制状态
}

//创建一个材质立方体
void DrawCube()
{
	//创建顶点缓存和索引缓存
	if (iDevice == nullptr)
	{
		::MessageBox(0, L"IDirectDevice is null", 0, 0);
		return;
	}
	iDevice->CreateVertexBuffer(
		12 * sizeof(NormalVertex),
		D3DUSAGE_WRITEONLY,
		NormalVertex::FVF,
		D3DPOOL_MANAGED,
		&iVB,
		0);
	if (iVB == nullptr)
	{
		::MessageBox(0, L"VertexBuffer create failed", 0, 0);
		return;
	}

	//填充缓存
	NormalVertex *vertice;
	iVB->Lock(0, 0, (void**)&vertice, 0);
	//front side
	vertice[0] = NormalVertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	vertice[1] = NormalVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f);
	vertice[2] = NormalVertex(1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	//left side
	vertice[3] = NormalVertex(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f);
	vertice[4] = NormalVertex(0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f);
	vertice[5] = NormalVertex(-1.0, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);
	//right side
	vertice[6] = NormalVertex(1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	vertice[7] = NormalVertex(0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f);
	vertice[8] = NormalVertex(1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f);
	//back side
	vertice[9] = NormalVertex(1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	vertice[10] = NormalVertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
	vertice[11] = NormalVertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);

	iVB->Unlock();

	
	//创建材质
	D3DMATERIAL9 Mtrl;
	Mtrl.Ambient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	Mtrl.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	Mtrl.Specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	Mtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	Mtrl.Power = 5.0f;

	iDevice->SetMaterial(&Mtrl);

	

}

//创建一个使用纹理的四边形
//file : "./tex.bmp";
void DrawTexQuad()
{
	//创建顶点索引
	if (iDevice == nullptr)
	{
		::MessageBox(0, L"IDirectDevice is null", 0, 0);
		return;
	}
	iDevice->CreateVertexBuffer(
		6 * sizeof(NormalTexVertex),
		D3DUSAGE_WRITEONLY,
		NormalTexVertex::FVF,
		D3DPOOL_MANAGED,
		&iVB,
		0);
	if (iVB == nullptr)
	{
		::MessageBox(0, L"VertexBuffer create failed", 0, 0);
		return;
	}

	NormalTexVertex* vertice = 0;
	iVB->Lock(0, 0, (void**)&vertice, 0);
	float len = 5;
	float bound = 1.0f;
	vertice[0] = NormalTexVertex(0.0f, 0.0f, len, 0.0f, 0.0f, -1.0f, 0.0f, bound);
	vertice[1] = NormalTexVertex(0.0f, len, len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertice[2] = NormalTexVertex(len, len, len, 0.0f, 0.0f, -1.0f, bound, 0.0f);
	vertice[3] = NormalTexVertex(0, 0, len, 0.0f, 0.0f, -1.0f, 0.0f, bound);
	vertice[4] = NormalTexVertex(len, len, len, 0.0f, 0.0f, -1.0f, bound, 0.0f);
	vertice[5] = NormalTexVertex(len, 0, len, 0.0f, 0.0f, -1.0f, bound, bound);


	iVB->Unlock();

	//加载纹理
	D3DXCreateTextureFromFile(iDevice, L"tex.dds", &iTex);
	iDevice->SetTexture(0, iTex);

	//纹理过滤器
	iDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	iDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	iDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	//寻址模式
	iDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	iDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

//创建背景和带有透明度的茶壶（需要开启融合计算）
void DrawTransparentTeapot()
{
	//创建茶壶mesh
	D3DXCreateTeapot(iDevice, &meshes[0], 0);

	//创建材质
	Mtrl.Diffuse.a = 0.5f;		//散射光alpha值
	Mtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Mtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Mtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Mtrl.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//设置alpha值的来源
	//从漫射光计算alpha
	iDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	iDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	//设置融合因子
	iDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	iDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);

	//创建背景
	DrawTexQuad();

}

//创建镜面的材质和纹理
void CreteMirror()
{
	if (iDevice == nullptr)
	{
		::MessageBox(0, L"IDirectDevice is null", 0, 0);
		return;
	}
	iDevice->CreateVertexBuffer(
		6 * sizeof(NormalTexVertex),
		D3DUSAGE_WRITEONLY,
		NormalTexVertex::FVF,
		D3DPOOL_MANAGED,
		&iVB,
		0);
	if (iVB == nullptr)
	{
		::MessageBox(0, L"VertexBuffer create failed", 0, 0);
		return;
	}

	NormalTexVertex* vertice = 0;
	iVB->Lock(0, 0, (void**)&vertice, 0);
	float len = 5;
	float bound = 1.0f;
	vertice[0] = NormalTexVertex(0.0f, 0.0f, len, 0.0f, 0.0f, -1.0f, 0.0f, bound);
	vertice[1] = NormalTexVertex(0.0f, len, len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertice[2] = NormalTexVertex(len, len, len, 0.0f, 0.0f, -1.0f, bound, 0.0f);
	vertice[3] = NormalTexVertex(0, 0, len, 0.0f, 0.0f, -1.0f, 0.0f, bound);
	vertice[4] = NormalTexVertex(len, len, len, 0.0f, 0.0f, -1.0f, bound, 0.0f);
	vertice[5] = NormalTexVertex(len, 0, len, 0.0f, 0.0f, -1.0f, bound, bound);


	iVB->Unlock();
	MirrorMtrl.Ambient = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	MirrorMtrl.Diffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	MirrorMtrl.Specular = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	MirrorMtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	//加载纹理
	D3DXCreateTextureFromFile(iDevice, L"mirror.jpg", &iTex);

	//纹理过滤器
	iDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	iDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	iDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	//寻址模式
	iDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	iDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//茶壶
	D3DXCreateTeapot(iDevice, &meshes[0], 0);

	TeapotMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeapotMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeapotMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeapotMtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

}

//创建茶壶
void CreateTeapot()
{
	D3DXCreateTeapot(iDevice, &meshes[0], 0);
	
	TeapotMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeapotMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeapotMtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeapotMtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	D3DXMatrixIdentity(&TeapotPos);
	D3DXMatrixTranslation(&TeapotPos, -1.0f, 2.0f, 0.0f);
}

//----------------光源--------------------------------
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	//TODO
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(D3DLIGHT9));

	return light;
}

D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	//TODO
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(D3DLIGHT9));
	return light;
}


//-----------------display------这里的函数放在beginscene和endscene之间-------------------------------
//实现镜面反射效果 绘制茶壶的映像
//目前在反射面的位置控制上还有点问题 看不到映像 需要灵活摄像机解决
void RenderMirror()
{
	//设置模板缓存
	iDevice->SetRenderState(D3DRS_STENCILENABLE, true);					//启用模板缓存
	iDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);			//比较运算函数(LSH与RSH如何进行比较)
	iDevice->SetRenderState(D3DRS_STENCILREF, 0x1);						//模板参考值
	iDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);				//模板掩码
	iDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);		//模板写掩码（用于屏蔽某些写入模板缓存的位）
	iDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);		//深度测试失败的处理
	iDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);		//像素模板测试失败的处理
	iDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);	//测试通过的处理

	//绘制反射镜面 但只绘制到模板缓存中 绘制完成后模板缓存对应镜面像素的值置为0x1(相当于在模板缓存中标识镜面)
	iDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);					//禁止对深度缓存的写操作
	iDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);				//启用深度融合计算
	iDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	iDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	iDevice->SetStreamSource(0, iVB, 0, sizeof(NormalTexVertex));
	iDevice->SetFVF(NormalTexVertex::FVF);
	iDevice->SetMaterial(&MirrorMtrl);

	D3DXMATRIX mtrx;
	D3DXMatrixIdentity(&mtrx);
	iDevice->SetTransform(D3DTS_WORLD, &mtrx);
	iDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	iDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	//更改模板检测设置
	iDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	iDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	//对物体进行映射
	D3DXMATRIX R0, R1, R2;
	D3DXPLANE plane(-0.5f, 0, -0.5f, 5.0f);
	D3DXMatrixReflect(&R0, &plane);		//获得plane平面反射成像的变换矩阵
	D3DXMatrixIdentity(&R1);
	//获得平移物体位置的变换矩阵
	R2 = R1 * R0;

	//开始绘制物体的映像
	iDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0); //清空深度缓存，否则映像会被镜子遮挡
	//因为是经镜子反射成像，源缓存来自镜子
	//iDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	iDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	iDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	//D3DXMatrixTranslation(&R2, -0.005f, 0.0f, 0.0f);
	iDevice->SetTransform(D3DTS_WORLD, &R2);
	iDevice->SetMaterial(&TeapotMtrl);
	iDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);	//剔除顺时钟三角面
	meshes[0]->DrawSubset(0);

	iDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	iDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	iDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//实现阴影效果
//需要初始化好茶壶和光源
void RenderShadow()
{
	//使用模板缓存防止二次融合
	//二次融合: 由于阴影是3d物体压成一个平面，若同一位置有多个物体重叠可能会比其他位置颜色更深
	iDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	iDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	iDevice->SetRenderState(D3DRS_STENCILREF, 0x0);
	iDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	iDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	iDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	iDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	iDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);		//写入后模板

	//计算阴影变换
	D3DXVECTOR4 lightDir(0.707f, -0.707f, 0.707f, 0.0f);
	D3DXPLANE plane(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXMATRIX R1, R2;
	D3DXMatrixShadow(&R1, &lightDir, &plane);	//计算阴影的矩阵
	R2 = TeapotPos * R1;
	iDevice->SetTransform(D3DTS_WORLD, &R2);

	//绘制阴影
	iDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	iDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	iDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl;
	mtrl.Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mtrl.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mtrl.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mtrl.Diffuse.a = 0.5f;		//阴影需要有透明度


	//iDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	iDevice->SetRenderState(D3DRS_ZENABLE, false);		//关闭深度缓存，防止深度冲突(z-fighing)
	iDevice->SetMaterial(&mtrl);
	D3DXMATRIX R;
	D3DXMatrixIdentity(&R);
	//iDevice->SetTransform(D3DTS_WORLD, &R);
	meshes[0]->DrawSubset(0);

	iDevice->SetRenderState(D3DRS_ZENABLE, true);
	iDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	iDevice->SetRenderState(D3DRS_STENCILENABLE, false);
}