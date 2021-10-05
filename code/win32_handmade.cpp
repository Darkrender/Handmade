#include <windows.h>
#define internal static
#define local_persist static
#define global_variable static

global_variable bool running; //TODO: This is a global for now.
global_variable BITMAPINFO bitmapInfo;
global_variable void *bitmapMemory;
global_variable HBITMAP bitmapHandle;
global_variable HDC bitmapDeviceContext;

LRESULT CALLBACK MainWindowCallback(HWND, UINT, WPARAM, LPARAM);
internal void ResizeDIBSection(int, int); // DIB = Device Independant Bitmap
void Win32UpdateWindow(HDC, int, int, int, int);

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
    WNDCLASS windowClass = {};
    // windowClass.style = CS_HREDRAW | CS_VREDRAW; // CS_HREDRAW and CS_VREDRAW redraws the entire window on horizontal or vertical resize
    windowClass.lpfnWndProc = MainWindowCallback;
    windowClass.hInstance = instance; // Could get the instance with GetModuleHandle(0)
    // WindowClass.hIcon;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    RegisterClass(&windowClass);
    
    HWND windowHandle = CreateWindowEx(
        0,
        windowClass.lpszClassName,
        "Handmade Hero",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0,
        0,
        instance,
        0
    );

    if (!windowHandle)
    {
        //TODO: Log some error here
        return 0;
    }

    // Main message loop:
    MSG message;
    running = true;
    while (running)
    {
        BOOL messageResult = GetMessage(&message, 0, 0, 0);

        if (messageResult > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            break;
        }

    }

    return 0;
}

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch(message)
    {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            ResizeDIBSection(width, height);
        } break;
        case WM_DESTROY:
        {
            //TODO: Handle this as an error -- recreate window?
            running = false;
        } break;
        case WM_CLOSE:
        {
            //TODO: Handle this with a message to the user?
            running = false;
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;
        case WM_PAINT:
        {
            OutputDebugStringA("WM_PAINT\n");
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);

            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;

            Win32UpdateWindow(deviceContext, x, y, width, height);
            
            EndPaint(window, &paint);
        } break;
        default:
        {
            // OutputDebugStringA("DEFAULT\n");
            result = DefWindowProc(window, message, wParam, lParam);
        } break;
    }

    return result;
}

internal void ResizeDIBSection(int width, int height)
{
    // TODO: Bulletproof this
    // Maybe dont free first, free after, then free first if that fails

    // Free our DIBSection
    if (bitmapHandle)
    {
        DeleteObject(bitmapHandle);
    }

    if (!bitmapDeviceContext)
    {
        //TODO: Should we recreate these under certain special circumstances?
        bitmapDeviceContext = CreateCompatibleDC(0);
    }

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    bitmapHandle = CreateDIBSection(
        bitmapDeviceContext,
        &bitmapInfo,
        DIB_RGB_COLORS,
        &bitmapMemory,
        0, 0
    );
}

internal void Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height)
{
    StretchDIBits(
        deviceContext,
        x, y, width, height,
        x, y, width, height,
        bitmapMemory,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}