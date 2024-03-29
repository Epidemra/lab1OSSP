// lab1.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "lab1.h"
#include <ctime>

#define MAX_LOADSTRING 100
#define FC_LOW 0
#define FC_HIGH 1
#define FC_NO 2
#define DSTEP 5

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];           // имя класса главного окна
RECT rect = {80, 100, 130, 150};
int color = 0;
bool isDrawed = false;
int wheelDelta = 0;
int d = 5;
int height, width;
RECT clientRect = {0};
bool isShifted = false;
int nTimerID;
float frequency = 250;
bool isFlying = false;
char degreeChose = FC_LOW;
bool isPicture = false;
bool isMouseDragging = false;

HANDLE hBitmap;
BITMAP Bitmap;

struct FlyInfo
{
	double degree;
	int lastX;
	int lastY;
	double dBottom;
	double dTop;
	double dRight;
	double dLeft;
} flyInfo;


// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //загрузка bmp
	hBitmap = LoadImage(NULL, L"spr1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBitmap) {
		MessageBox(NULL, L"BMP not found!", L"Error", MB_OK);
		return 0;
	}
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//движение вверх(норм фильм)
void goUp(float step) 
{
	if (rect.top == 0)
	{
		rect.top = 25;
		rect.bottom = rect.top + height;
	}
	else
	{
		rect.top -= step;
		if (rect.top <= 0)
		{
			rect.top = 0;
			rect.bottom = rect.top + height + step;
		}
		rect.bottom -= step;
	}
}

//движение вниз
void goDown(float step)
{
	if (rect.bottom == clientRect.bottom)
	{
		rect.bottom = clientRect.bottom - 25;
		rect.top = rect.bottom - height;
	}
	else
	{
		rect.bottom += step;
		if (rect.bottom >= clientRect.bottom)
		{
			rect.bottom = clientRect.bottom;
			rect.top = rect.bottom - height - step;
		}
		rect.top += step;
	}
}

//движение налево
void goLeft(float step)
{
	if (rect.left == 0)
	{
		rect.left = 25;
		rect.right = rect.left + width;
	}
	else
	{
		rect.left -= step;
		if (rect.left <= 0)
		{
			rect.left = 0;
			rect.right = rect.left + width + step;
		}
		rect.right -= step;
	}
}

//движение вправо
void goRight(float step)
{
	if (rect.right == clientRect.right)
	{
		rect.right = clientRect.right - 25;
		rect.left = rect.right - width;
	}
	else
	{
		rect.right += step;
		if (rect.right >= clientRect.right)
		{
			rect.right = clientRect.right;
			rect.left = rect.right - width - step;
		}
		rect.left += step;
	}
}

//процедура для обновления координат при полёте
void Update() 
{
	//удар верхом
	if (rect.top == 0)
	{
		goDown(d);
		flyInfo.dBottom = d;
		flyInfo.dTop = 0;
		if (flyInfo.dLeft != 0)
		{
			goLeft(d);
			flyInfo.dLeft = -d;
			flyInfo.dRight = 0;
		}
		else
		{
			goRight(d);
			flyInfo.dLeft = 0;
			flyInfo.dRight = d;
		}
		return;
	}

	//удар левым боком
	if (rect.left == 0)
	{
		goRight(d);
		flyInfo.dRight = d;
		flyInfo.dLeft = 0;
		if (flyInfo.dBottom != 0)
		{
			goDown(d);
			flyInfo.dBottom = d;
			flyInfo.dTop = 0;
		}
		else
		{
			goUp(d);
			flyInfo.dTop = d;
			flyInfo.dBottom= 0;
		}
		return;
	}

	//удар правым боком
	if (rect.right == clientRect.right)
	{
		goLeft(d);
		flyInfo.dLeft = -d;
		flyInfo.dRight = 0;
		if (flyInfo.dBottom != 0)
		{
			goDown(d);
			flyInfo.dBottom = d;
			flyInfo.dTop = 0;
		}
		else
		{
			goUp(d);
			flyInfo.dTop = d;
			flyInfo.dBottom = 0;
		}
		return;
	}

	//удар низом
	if (rect.bottom == clientRect.bottom)
	{
		goUp(d);
		flyInfo.dTop = -d;
		flyInfo.dBottom = 0;
		if (flyInfo.dLeft != 0)
		{
			goLeft(d);
			flyInfo.dLeft = -d;
			flyInfo.dRight = 0;
		}
		else
		{
			goRight(d);
			flyInfo.dLeft = 0;
			flyInfo.dRight = d;
		}
		return;
	}
	
	//просто полёт
	if (flyInfo.dBottom != 0)
		goDown(d);
	if (flyInfo.dLeft != 0)
		goLeft(d);
	if (flyInfo.dRight != 0)
		goRight(d);
	if (flyInfo.dTop != 0)
		goUp(d);
}
//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      100, 100, 500, 400, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, hCompatibleDC;
	HANDLE hOldBitmap;
	int oldWheel = wheelDelta;
	int xPos = 0, yPos = 0, prevXPos = 0, prevYPos = 0;

	GetClientRect(hWnd, &clientRect);

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
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
			hdc = BeginPaint(hWnd, &ps);
			if (!isPicture)
			{
				//прямоугольник
				FillRect(hdc, &rect, HBRUSH(CreateSolidBrush(RGB(255, 0, 55))));
			}
			else
			{
				//bmp
				hCompatibleDC = CreateCompatibleDC(hdc);
				hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
				StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hCompatibleDC, 13, 0, Bitmap.bmWidth - 25, Bitmap.bmHeight - 5, SRCCOPY);
				SelectObject(hCompatibleDC, hOldBitmap);
			}
			//ValidateRect(hWnd, NULL);
			
			EndPaint(hWnd, &ps);
        }
        break;
	case WM_KEYDOWN: //обработка нажатий на кнопки
		height = rect.bottom - rect.top;
		width = rect.right - rect.left;
		switch (wParam)
		{
		case VK_F1:
			nTimerID = SetTimer(hWnd, 1, frequency, NULL);
			flyInfo.lastX = rect.left;
			flyInfo.lastY = rect.top;
			flyInfo.dTop = -d;
			flyInfo.dLeft = -d;
			flyInfo.dBottom = 0;
			flyInfo.dRight = 0;
			flyInfo.degree = 45;
			isFlying = true;
			break;
		case VK_F2:
			if (isFlying)
			{
				KillTimer(hWnd, 1);
				if (frequency > 4)
					frequency *= 0.85;
				nTimerID = SetTimer(hWnd, 1, frequency, NULL);
			}
			else
				if(d < 20)
					d += DSTEP;
			break;
		case VK_F3:
			if (isFlying)
			{
				KillTimer(hWnd, 1);
				if (frequency < 600)
					frequency /= 0.85;
				nTimerID = SetTimer(hWnd, 1, frequency, NULL);
			}
			else
				if (d > DSTEP)
					d -= DSTEP;
			break;
		case VK_F4:
			isFlying = false;
			KillTimer(hWnd, 1);
			break;
		case VK_F5:
			isPicture = !isPicture;
			InvalidateRect(hWnd, NULL, true);
			break;
		case VK_SHIFT:
			isShifted = true;
			break;
		case VK_UP:
			goUp(d);
			InvalidateRect(hWnd, NULL, true);
			break;
		case VK_DOWN:
			goDown(d);
			InvalidateRect(hWnd, NULL, true);
			break;
		case VK_RIGHT:
			goRight(d);
			InvalidateRect(hWnd, NULL, true);
			//ReleaseDC(hWnd, hdc);
			break;
		case VK_LEFT:
			goLeft(d);
			InvalidateRect(hWnd, NULL, true);
			break;
		}
		//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case WM_KEYUP: //поднятие кнопки
		if (wParam == VK_SHIFT)
			isShifted = false;
		break;
	case WM_MOUSEWHEEL: //прокрутка колеса
		wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
			if (isShifted)
				goRight(d);
			else
				goUp(d);
		for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
			if (isShifted)
				goLeft(d);
			else
				goDown(d);
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_LBUTTONDOWN:  //нажатие левой кнопки мыши
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		if ((xPos >= rect.left && xPos <= rect.right) &&
			(yPos >= rect.top && yPos <= rect.bottom) &&
			!isFlying) 
		{
			prevXPos = xPos;
			prevYPos = yPos;
			isMouseDragging = true;
		}			
		break;
	case WM_MOUSEMOVE: //движение мышки 
		height = rect.bottom - rect.top;
		width = rect.right - rect.left;
		if (isMouseDragging)
		{
			xPos = LOWORD(lParam);
			yPos = HIWORD(lParam);
			rect.top = yPos;
			rect.bottom = yPos + height;
			rect.right = xPos + width;
			rect.left = xPos;
			prevXPos = xPos;
			prevYPos = yPos;
			InvalidateRect(hWnd, NULL, true);
		}
		break;
	case WM_LBUTTONUP: //поднятие левой кнопки мыши
		isMouseDragging = false;
		break;
	case WM_TIMER: //таймер
		Update();
		InvalidateRect(hWnd, NULL, true);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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