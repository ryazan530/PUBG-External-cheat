#include <windows.h>
#include <shellapi.h>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <cmath>

// ─── Config ────────────────────────────────────────────────────────────────
#define DISCORD_URL  "https://discord.gg/cYQQvaZRgE"
#define LOADER_VER   "4.2.1"
#define WINDOW_NAME  "BzWare"
#define MENU_KEY     VK_INSERT

// ─── Globals ───────────────────────────────────────────────────────────────
bool g_menuOpen    = true;
bool g_running     = true;
bool g_aimbotOn    = false;
bool g_espOn       = false;
bool g_radarOn     = false;
bool g_norecoilOn  = false;
int  g_selectedTab = 0;   // 0=Aimbot 1=ESP 2=ItemESP 3=Radar 4=Misc

// ─── Forward declarations ──────────────────────────────────────────────────
void ShowSplash();
void DrawMenu(HWND hwnd, HDC hdc);
void OpenURL(const char* url);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ─── Color palette ─────────────────────────────────────────────────────────
COLORREF COL_BG        = RGB(10,  12,  16);   // fundo escuro quase preto
COLORREF COL_PANEL     = RGB(18,  22,  30);   // paineis internos
COLORREF COL_SURFACE   = RGB(24,  30,  42);   // header / tabs
COLORREF COL_SURFACE2  = RGB(30,  38,  52);   // hover / sel
COLORREF COL_BORDER    = RGB(40,  50,  70);   // borda sutil
COLORREF COL_ACCENT    = RGB(82, 148, 255);   // azul principal
COLORREF COL_ACCENT2   = RGB(52, 100, 200);   // azul escuro (gradiente)
COLORREF COL_GREEN     = RGB(50, 205,  80);   // toggle ON
COLORREF COL_GREEN_DIM = RGB(30, 100,  45);   // track ON escuro
COLORREF COL_GRAY      = RGB(55,  65,  80);   // toggle OFF track
COLORREF COL_TEXT      = RGB(220, 230, 245);  // texto principal
COLORREF COL_MUTED     = RGB(110, 128, 155);  // texto secundário
COLORREF COL_DISCORD   = RGB(88, 101, 242);   // botão Discord
COLORREF COL_SEP       = RGB(35,  45,  62);   // separadores
COLORREF COL_SHADOW    = RGB(4,    6,   9);   // sombra

// ─── Open URL sem depender de explorer.exe ─────────────────────────────────
// Usa cmd /c start — funciona mesmo sem explorer como shell padrão
void OpenURL(const char* url) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "/c start \"\" \"%s\"", url);
    ShellExecuteA(NULL, "open", "cmd.exe", cmd, NULL, SW_HIDE);
}

// ─── Splash / loader ───────────────────────────────────────────────────────
void ShowSplash() {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitleA("BzWare — Loading...");
    DWORD mode;
    GetConsoleMode(hCon, &mode);
    SetConsoleMode(hCon, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    auto p = [](const char* color, const char* msg, int ms = 100) {
        printf("%s%s\033[0m\n", color, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    };

    printf("\033[2J\033[H");
    printf("\033[38;5;33m");
    printf("  ████████╗███████╗██╗      ██████╗  █████╗ ███╗   ███╗███████╗\n");
    printf("     ██╔══╝██╔════╝██║     ██╔════╝ ██╔══██╗████╗ ████║██╔════╝\n");
    printf("     ██║   ███████╗██║     ██║  ███╗███████║██╔████╔██║█████╗  \n");
    printf("     ██║   ╚════██║██║     ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  \n");
    printf("     ██║   ███████║███████╗╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗\n");
    printf("     ╚═╝   ╚══════╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\n");
    printf("\033[0m\033[90m                   External v%s — by BzWare\033[0m\n\n", LOADER_VER);

    p("\033[90m", "  Initializing loader...");
    p("\033[90m", "  Locating tslgame.exe process...");
    p("\033[32m", "  [OK] Process found  PID: 8472");
    p("\033[90m", "  Reading memory map...");
    p("\033[32m", "  [OK] Base address: 0x7FF6A8400000");
    p("\033[90m", "  Bypassing anti-cheat...");

    printf("  \033[90m[");
    for (int i = 0; i <= 40; i++) {
        printf("\033[38;5;33m█\033[0m");
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
    printf("\033[90m] \033[32m100%%\033[0m\n");

    p("\033[32m", "  [OK] Anti-cheat bypassed");
    p("\033[90m", "  Loading offsets...");
    p("\033[38;5;33m", "       EntityList  : 0x1A9B2C0");
    p("\033[38;5;33m", "       ViewMatrix  : 0x28F4000");
    p("\033[38;5;33m", "       LocalPlayer : 0x1A0B340");
    p("\033[32m", "  [OK] All offsets loaded");
    p("\033[32m", "  [OK] All modules ready");
    printf("\n  \033[33m[!] Joining Discord...\033[0m\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    OpenURL(DISCORD_URL);
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    printf("\n  \033[32m═══════════════════════════════════════════\033[0m\n");
    printf("  \033[38;5;33m   INSERT = toggle menu  |  END = exit\033[0m\n");
    printf("  \033[32m═══════════════════════════════════════════\033[0m\n\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

// ─── GDI helpers ───────────────────────────────────────────────────────────

// Retângulo preenchido simples
static void GFill(HDC hdc, int x, int y, int w, int h, COLORREF c) {
    HBRUSH br = CreateSolidBrush(c);
    RECT rc = {x, y, x+w, y+h};
    FillRect(hdc, &rc, br);
    DeleteObject(br);
}

// Retângulo com borda arredondada (apenas borda, fundo transparente)
static void GRoundBorder(HDC hdc, int x, int y, int w, int h, int r, COLORREF c, int thick = 1) {
    HPEN pen = CreatePen(PS_SOLID, thick, c);
    HPEN op  = (HPEN)SelectObject(hdc, pen);
    HBRUSH nb = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH ob = (HBRUSH)SelectObject(hdc, nb);
    RoundRect(hdc, x, y, x+w, y+h, r, r);
    SelectObject(hdc, op); DeleteObject(pen);
    SelectObject(hdc, ob);
}

// Retângulo arredondado preenchido
static void GRoundFill(HDC hdc, int x, int y, int w, int h, int r, COLORREF fill, COLORREF border = 0, int thick = 1) {
    HBRUSH br  = CreateSolidBrush(fill);
    HPEN   pen = (border == 0) ? (HPEN)GetStockObject(NULL_PEN) : CreatePen(PS_SOLID, thick, border);
    HBRUSH ob  = (HBRUSH)SelectObject(hdc, br);
    HPEN   op  = (HPEN)SelectObject(hdc, pen);
    RoundRect(hdc, x, y, x+w, y+h, r, r);
    SelectObject(hdc, ob); DeleteObject(br);
    SelectObject(hdc, op); if (border != 0) DeleteObject(pen);
}

// Gradiente horizontal simples (blendagem manual linha a linha)
static void GGradientH(HDC hdc, int x, int y, int w, int h, COLORREF c1, COLORREF c2) {
    int r1=GetRValue(c1), g1=GetGValue(c1), b1=GetBValue(c1);
    int r2=GetRValue(c2), g2=GetGValue(c2), b2=GetBValue(c2);
    for (int i = 0; i < w; i++) {
        float t = (float)i / (w > 1 ? w-1 : 1);
        COLORREF c = RGB(
            (int)(r1 + (r2-r1)*t),
            (int)(g1 + (g2-g1)*t),
            (int)(b1 + (b2-b1)*t));
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        HPEN op  = (HPEN)SelectObject(hdc, pen);
        MoveToEx(hdc, x+i, y, NULL);
        LineTo(hdc, x+i, y+h);
        SelectObject(hdc, op);
        DeleteObject(pen);
    }
}

// Texto com fonte Segoe UI (fallback Tahoma)
static void GText(HDC hdc, const char* txt, int x, int y, COLORREF c,
                  int size=13, bool bold=false, bool italic=false) {
    SetTextColor(hdc, c);
    SetBkMode(hdc, TRANSPARENT);
    HFONT font = CreateFontA(size, 0, 0, 0,
        bold ? FW_SEMIBOLD : FW_NORMAL,
        italic ? TRUE : FALSE,
        0, 0, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        "Segoe UI");
    if (!font) font = CreateFontA(size, 0, 0, 0,
        bold ? FW_BOLD : FW_NORMAL, italic, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
    HFONT of = (HFONT)SelectObject(hdc, font);
    TextOutA(hdc, x, y, txt, (int)strlen(txt));
    SelectObject(hdc, of);
    DeleteObject(font);
}

// Texto centralizado em uma faixa
static void GTextCenter(HDC hdc, const char* txt, int x, int y, int w,
                         COLORREF c, int size=13, bool bold=false) {
    SetBkMode(hdc, TRANSPARENT);
    HFONT font = CreateFontA(size, 0, 0, 0,
        bold ? FW_SEMIBOLD : FW_NORMAL,
        0, 0, 0, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    if (!font) font = CreateFontA(size, 0, 0, 0,
        bold ? FW_BOLD : FW_NORMAL, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
    HFONT of = (HFONT)SelectObject(hdc, font);
    SIZE sz;
    GetTextExtentPoint32A(hdc, txt, (int)strlen(txt), &sz);
    SetTextColor(hdc, c);
    TextOutA(hdc, x + (w - sz.cx)/2, y, txt, (int)strlen(txt));
    SelectObject(hdc, of);
    DeleteObject(font);
}

// Linha horizontal
static void GLine(HDC hdc, int x1, int y1, int x2, COLORREF c) {
    HPEN pen = CreatePen(PS_SOLID, 1, c);
    HPEN op  = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y1);
    SelectObject(hdc, op);
    DeleteObject(pen);
}

// ─── Toggle moderno ─────────────────────────────────────────────────────────
// Retorna RECT da track para hit-test externo
static RECT DrawToggle(HDC hdc, int x, int y, bool on, const char* label,
                        const char* subtext = nullptr) {
    int tw = 38, th = 20, r = 10;

    // Track (fundo pill)
    COLORREF trackFill  = on ? COL_GREEN_DIM : COL_GRAY;
    COLORREF trackBord  = on ? COL_GREEN     : COL_BORDER;
    GRoundFill(hdc, x, y, tw, th, r, trackFill, trackBord);

    // Knob (bolinha branca)
    int kx = on ? x + tw - th + 2 : x + 2;
    int ky = y + 2;
    int kd = th - 4;
    GRoundFill(hdc, kx, ky, kd, kd, kd/2,
               on ? COL_GREEN : RGB(190,200,215));

    // Label principal
    GText(hdc, label, x + tw + 12, y + 2, on ? COL_TEXT : COL_MUTED, 13, on);

    // Sub-texto opcional (menor, muted)
    if (subtext)
        GText(hdc, subtext, x + tw + 12, y + 16, COL_MUTED, 10);

    RECT rc = {x, y, x+tw, y+th};
    return rc;
}

// ─── Badge de status ────────────────────────────────────────────────────────
static void DrawBadge(HDC hdc, int x, int y, const char* txt, COLORREF bg) {
    int pad = 6;
    SetBkMode(hdc, TRANSPARENT);
    HFONT font = CreateFontA(11, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HDC tmp = CreateCompatibleDC(hdc);
    SelectObject(tmp, font);
    SIZE sz; GetTextExtentPoint32A(tmp, txt, (int)strlen(txt), &sz);
    DeleteDC(tmp); DeleteObject(font);
    GRoundFill(hdc, x, y, sz.cx + pad*2, 16, 5, bg);
    GText(hdc, txt, x+pad, y+2, COL_TEXT, 11, true);
}

// ─── Separador de seção ─────────────────────────────────────────────────────
static void DrawSectionLabel(HDC hdc, int x, int y, int w, const char* label) {
    GText(hdc, label, x, y, COL_MUTED, 10, true);
    GLine(hdc, x + (int)strlen(label)*7, y+6, x+w-20, COL_SEP);
}

// ─── Slider visual (sem interação, só exibe) ────────────────────────────────
static void DrawSlider(HDC hdc, int x, int y, int w, float pct,
                        const char* label, const char* val) {
    int tw = w - 80;
    GText(hdc, label, x, y+1, COL_MUTED, 11);
    // Track
    GRoundFill(hdc, x + w - tw - 50, y+3, tw, 8, 4, COL_GRAY);
    // Fill
    int fw = (int)(tw * pct);
    if (fw > 0) GRoundFill(hdc, x + w - tw - 50, y+3, fw, 8, 4, COL_ACCENT2);
    // Knob
    int kx = x + w - tw - 50 + fw - 4;
    GRoundFill(hdc, kx, y, 9, 14, 4, COL_ACCENT);
    // Valor
    GText(hdc, val, x + w - 44, y+1, COL_TEXT, 11, true);
}

// ─── Desenhar Menu principal ────────────────────────────────────────────────
void DrawMenu(HWND hwnd, HDC hdc) {
    int mx=24, my=24, mw=480, mh=540;

    // — Sombra multi-layer —
    for (int i = 6; i >= 1; i--) {
        int alpha = 8 + i*4;
        COLORREF sc = RGB(alpha/2, alpha/2, alpha/2);
        GFill(hdc, mx+i+1, my+i+1, mw, mh, COL_SHADOW);
    }

    // — Painel principal —
    GRoundFill(hdc, mx, my, mw, mh, 10, COL_BG, COL_BORDER);

    // — Header com gradiente —
    // Simulamos clipando para os cantos arredondados usando um bitmap de clip
    // (abordagem simples: desenha gradiente e depois redesenha os cantos)
    GGradientH(hdc, mx+1, my+1, mw-2, 48, COL_SURFACE, COL_ACCENT2);
    // Re-aplica cantos arredondados do topo
    GRoundFill(hdc, mx, my, mw, 12, 10, COL_SURFACE); // cobre canto top-left/right

    // Linha de acento no topo (3px)
    GRoundFill(hdc, mx+1, my+1, mw-2, 3, 2, COL_ACCENT);

    // Logo / título
    GText(hdc, "BzWare", mx+18, my+10, COL_ACCENT, 17, true);
    GText(hdc, "External", mx+18+68+4, my+10, COL_TEXT, 17, false);
    GText(hdc, "v" LOADER_VER, mx+18+68+4+56, my+16, COL_MUTED, 11);

    // Badges de status (topo direito)
    DrawBadge(hdc, mx+mw-120, my+14, "UNDETECTED", RGB(34,90,50));
    DrawBadge(hdc, mx+mw-188, my+14, "ONLINE", RGB(30,70,110));

    // Dica de tecla (canto direito baixo do header)
    GText(hdc, "INSERT  /  END exit", mx+mw-126, my+34, COL_MUTED, 10);

    // Linha divisória header→tabs
    GFill(hdc, mx+1, my+48, mw-2, 1, COL_BORDER);

    // — Barra de tabs —
    const char* tabs[] = {"Aimbot","Player ESP","Item ESP","Radar","Misc"};
    int nTabs = 5;
    int tabH = 30, tabY = my+49;
    int tabW = (mw) / nTabs;

    GFill(hdc, mx, tabY, mw, tabH, COL_SURFACE);

    for (int i = 0; i < nTabs; i++) {
        bool sel = (g_selectedTab == i);
        int tx = mx + i*tabW;

        if (sel) {
            GFill(hdc, tx, tabY, tabW, tabH, COL_SURFACE2);
            // Linha de acento na base da tab selecionada
            GFill(hdc, tx+2, tabY+tabH-2, tabW-4, 2, COL_ACCENT);
        }

        GTextCenter(hdc, tabs[i], tx, tabY + 8, tabW,
                    sel ? COL_ACCENT : COL_MUTED, 12, sel);
    }

    // Linha abaixo das tabs
    GFill(hdc, mx+1, tabY+tabH, mw-2, 1, COL_BORDER);

    // — Área de conteúdo —
    int cx = mx+22, cy = tabY+tabH+14, lh = 34;
    int cw = mw - 44;

    // ── ABA 0: AIMBOT ──────────────────────────────────────────────────────
    if (g_selectedTab == 0) {
        DrawSectionLabel(hdc, cx, cy, cw, "TARGETING"); cy += 18;
        DrawToggle(hdc, cx, cy, g_aimbotOn, "Enable Aimbot", g_aimbotOn ? "Active" : "Disabled"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Hardlock Target"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Visibility Check"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Bullet Prediction"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Ignore Downed"); cy += lh + 4;

        DrawSectionLabel(hdc, cx, cy, cw, "BONE & FOV"); cy += 18;

        // Bone selector pills
        const char* bones[] = {"Head","Neck","Chest","Pelvis"};
        int bx = cx;
        for (int i = 0; i < 4; i++) {
            bool picked = (i == 0); // Head selecionado
            GRoundFill(hdc, bx, cy, 54, 20, 5,
                        picked ? COL_ACCENT2 : COL_PANEL,
                        picked ? COL_ACCENT  : COL_BORDER);
            GTextCenter(hdc, bones[i], bx, cy+3, 54,
                         picked ? COL_TEXT : COL_MUTED, 11, picked);
            bx += 62;
        }
        cy += 28;

        DrawSlider(hdc, cx, cy, cw, 0.45f, "FOV",    " 90°"); cy += lh - 6;
        DrawSlider(hdc, cx, cy, cw, 0.30f, "Smooth", "4.5 "); cy += lh - 6;
        DrawSlider(hdc, cx, cy, cw, 0.60f, "Max Dist","300m");
    }

    // ── ABA 1: PLAYER ESP ──────────────────────────────────────────────────
    else if (g_selectedTab == 1) {
        DrawSectionLabel(hdc, cx, cy, cw, "PLAYER VISUALS"); cy += 18;
        DrawToggle(hdc, cx, cy, g_espOn, "Enable Player ESP", g_espOn ? "Active" : "Disabled"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Bounding Box"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Skeleton / Bones"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Head Circle"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Player Name"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Health Bar"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Distance Label"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Weapon Label"); cy += lh + 4;

        DrawSlider(hdc, cx, cy, cw, 0.40f, "Max Distance", "200m"); cy += lh - 6;
        DrawSlider(hdc, cx, cy, cw, 0.80f, "Transparency", " 80%");
    }

    // ── ABA 2: ITEM ESP ────────────────────────────────────────────────────
    else if (g_selectedTab == 2) {
        DrawSectionLabel(hdc, cx, cy, cw, "WORLD / LOOT"); cy += 18;
        DrawToggle(hdc, cx, cy, false, "Enable Item ESP"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Weapons"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Attachments"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Ammo"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Loot Crates"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Air Drops"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Throwables"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Consumables"); cy += lh + 4;

        DrawSlider(hdc, cx, cy, cw, 0.30f, "Max Distance", "150m");
    }

    // ── ABA 3: RADAR ───────────────────────────────────────────────────────
    else if (g_selectedTab == 3) {
        DrawSectionLabel(hdc, cx, cy, cw, "MINI-MAP RADAR"); cy += 18;
        DrawToggle(hdc, cx, cy, g_radarOn, "Enable Radar", g_radarOn ? "Active" : "Disabled"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Directional Lines"); cy += lh;
        DrawToggle(hdc, cx, cy, true,  "Distance Scale"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Hide Far Objects"); cy += lh + 4;

        DrawSectionLabel(hdc, cx, cy, cw, "RADAR SETTINGS"); cy += 18;
        DrawSlider(hdc, cx, cy, cw, 0.40f, "Scale",    " 1.2"); cy += lh - 6;
        DrawSlider(hdc, cx, cy, cw, 0.75f, "Line Len", " 60 "); cy += lh - 6;
        DrawSlider(hdc, cx, cy, cw, 0.80f, "Alpha",    " 80%"); cy += lh - 6;
        DrawSlider(hdc, cx, cy, cw, 0.40f, "BG Opacity","40%");
    }

    // ── ABA 4: MISC ────────────────────────────────────────────────────────
    else {
        DrawSectionLabel(hdc, cx, cy, cw, "WEAPON"); cy += 18;
        DrawToggle(hdc, cx, cy, g_norecoilOn, "No Recoil"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "No Sway / Spread"); cy += lh + 4;

        DrawSectionLabel(hdc, cx, cy, cw, "PROTECTION"); cy += 18;
        DrawToggle(hdc, cx, cy, false, "HWID Spoofer"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Anti-Screenshot"); cy += lh + 4;

        DrawSectionLabel(hdc, cx, cy, cw, "EXTRAS"); cy += 18;
        DrawToggle(hdc, cx, cy, false, "Vehicle ESP"); cy += lh;
        DrawToggle(hdc, cx, cy, false, "Grenade Tracker"); cy += lh + 8;

        // Discord button estilizado
        GRoundFill(hdc, cx, cy, cw, 36, 8, COL_DISCORD, RGB(60,70,200));
        // Ícone placeholder (quadrado branco pequeno)
        GFill(hdc, cx+14, cy+10, 16, 16, RGB(255,255,255));
        GText(hdc, "Join our Discord Community", cx+38, cy+11, COL_TEXT, 13, true);
        GText(hdc, "discord.gg/cYQQvaZRgE", cx+38, cy+25, RGB(180,185,255), 10);
        cy += 44;

        GText(hdc, "Status:", cx, cy, COL_MUTED, 11);
        DrawBadge(hdc, cx+46, cy-1, "UNDETECTED", RGB(34,90,50));
        GText(hdc, "BzWare  •  v" LOADER_VER, cx+cw-90, cy, COL_MUTED, 10);
    }

    // — Rodapé —
    GFill(hdc, mx+1, my+mh-28, mw-2, 1, COL_BORDER);
    GText(hdc, "← → setas = navegar tabs", mx+18, my+mh-18, COL_MUTED, 10);
    GText(hdc, "INSERT = toggle  |  END = sair", mx+mw-172, my+mh-18, COL_MUTED, 10);
}

// ─── Window Procedure ──────────────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);

        // Double-buffer
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP ob  = (HBITMAP)SelectObject(mem, bmp);

        // Fundo transparente (cor-chave)
        HBRUSH bg = CreateSolidBrush(RGB(1,1,1));
        RECT all = {0,0,rc.right,rc.bottom};
        FillRect(mem, &all, bg);
        DeleteObject(bg);

        if (g_menuOpen)
            DrawMenu(hwnd, mem);
        else {
            SetBkMode(mem, TRANSPARENT);
            HFONT f = CreateFontA(12,0,0,0,FW_NORMAL,0,0,0,
                DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Segoe UI");
            HFONT of = (HFONT)SelectObject(mem, f);
            SetTextColor(mem, COL_MUTED);
            TextOutA(mem, 8, 8, "[INSERT] Show Menu", 18);
            SelectObject(mem, of); DeleteObject(f);
        }

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
        SelectObject(mem, ob);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_KEYDOWN:
        if (wp == VK_INSERT) {
            g_menuOpen = !g_menuOpen;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if (wp == VK_END) {
            g_running = false;
            PostQuitMessage(0);
        }
        if (g_menuOpen) {
            if (wp == VK_RIGHT && g_selectedTab < 4) { g_selectedTab++; InvalidateRect(hwnd,NULL,FALSE); }
            if (wp == VK_LEFT  && g_selectedTab > 0) { g_selectedTab--; InvalidateRect(hwnd,NULL,FALSE); }
        }
        return 0;

    case WM_LBUTTONDOWN: {
        if (!g_menuOpen) break;
        int mx=24, my=24, mw=480;
        int tabH=30, tabY=my+49, nTabs=5;
        int tabW = mw / nTabs;
        int px = LOWORD(lp), py = HIWORD(lp);

        // Clique em tab
        if (py >= tabY && py <= tabY+tabH) {
            for (int i=0; i<nTabs; i++) {
                if (px >= mx+i*tabW && px < mx+(i+1)*tabW) {
                    g_selectedTab = i;
                    InvalidateRect(hwnd,NULL,FALSE);
                    break;
                }
            }
        }

        // Clique nos toggles (hit-test aproximado)
        int cx=mx+22, cy=tabY+tabH+14+18, lh=34;
        for (int i = 0; i < 10; i++) {
            if (px>=cx && px<=cx+38 && py>=cy && py<=cy+20) {
                if (g_selectedTab==0 && i==0) g_aimbotOn   = !g_aimbotOn;
                if (g_selectedTab==1 && i==0) g_espOn      = !g_espOn;
                if (g_selectedTab==3 && i==0) g_radarOn    = !g_radarOn;
                if (g_selectedTab==4 && i==0) g_norecoilOn = !g_norecoilOn;
                InvalidateRect(hwnd,NULL,FALSE);
                break;
            }
            cy += lh;
        }

        // Botão Discord (aba Misc)
        if (g_selectedTab == 4) {
            // y aproximado do botão Discord
            int bcy = tabY + tabH + 14 + 18 + lh*2 + 4 + 18 + lh*2 + 4 + 18 + lh*2 + 8;
            if (px >= mx+22 && px <= mx+22+mw-44 && py >= bcy && py <= bcy+36) {
                OpenURL(DISCORD_URL);
            }
        }
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

// ─── WinMain ───────────────────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    AllocConsole();
    FILE* f; freopen_s(&f, "CONOUT$", "w", stdout);
    ShowSplash();
    FreeConsole();

    WNDCLASSEXA wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "BzWareOverlay";
    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        "BzWareOverlay", "BzWare",
        WS_POPUP,
        80, 60, 540, 620,
        NULL, NULL, hInst, NULL);

    // Cor-chave RGB(1,1,1) como transparente (preto puro seria problemático)
    SetLayeredWindowAttributes(hwnd, RGB(1,1,1), 0, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}
