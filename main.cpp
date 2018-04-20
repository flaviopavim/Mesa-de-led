#include <windows.h>
#include <gl/gl.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int width = 200;
int height = 300;

void pixel(int l, int t, char* hex) {
    glPointSize(1.0);
    glBegin(GL_POINTS);
    
    long number = (long) strtol(&hex[0], NULL, 16);
    int r = ((number >> 16) & 0xFF);
    int g = ((number >> 8) & 0xFF);
    int b = ((number) & 0xFF);
    
    glColor3f(map(r, 0, 255, 0, 1), map(g, 0, 255, 0, 1), map(b, 0, 255, 0, 1));
    glVertex3f(map(l, 1, width, -1, 1), map(t, 1, height, 1, -1), 0.0f);
    glEnd();
}

bool Matrix[10][15]; //obstáculos
bool MatrixPersonagem[10][15]; //pras peças ou outros recursos
char* Cor[10][15];

void square(int x, int y, char* hex) {
	if (hex!="000000") {
		Matrix[x][y]=true; //tem pixel
	} else {
		Matrix[x][y]=false; //não tem pixel (usar isso pra jogos)
	}
	Cor[x][y]=hex;
    int x_ = (x - 1)*20;
    int y_ = (y - 1)*20;
    for (int a = 1 + x_; a <= 20 + x_; a++) {
        for (int b = 1 + y_; b <= 20 + y_; b++) {
            pixel(a, b, hex);
        }
    }
}

char* randColor() {
	int len=6;
	char* s=new char[len];
    static const char alphanum[] =
        "0123456789"
        "abcdef";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
    return s;
}


void randPixel() {
	int x=rand()%10+1; // Randomizing the number between 1-10.
    int y=rand()%15+1; // Randomizing the number between 1-15.
    square(x, y, randColor());
}


void screen() {
    for (int i=0; i<=100; i++) {
    	randPixel();
	}
}

void refreshScreen(HDC hDC) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    screen();
    SwapBuffers(hDC);
    Sleep(1000);
}

int WINAPI WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int iCmdShow) {
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Hunter";
    RegisterClass(&wc);
    hWnd = CreateWindow(
            "Hunter", "Painel de LED",
            WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
            500, 200, width, height,
            NULL, NULL, hInstance, NULL);
    EnableOpenGL(hWnd, &hDC, &hRC);
    while (!bQuit) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            refreshScreen(hDC);
        }
    }
    DisableOpenGL(hWnd, hDC, hRC);
    DestroyWindow(hWnd);
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            return 0;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    return 0;
            }
            return 0;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;
    *hDC = GetDC(hWnd);
    ZeroMemory(&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
            PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}
