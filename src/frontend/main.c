#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../include/common.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define COLOR_BG "#CC0000"     // Soviet Red
#define COLOR_FG "#FFD700"     // Gold
#define COLOR_INPUT "#FFFFFF"  // White for inputs
#define COLOR_TEXT_INPUT "#000000" // Black text in inputs

// Application States
typedef enum {
    STATE_LOGIN,
    STATE_DASHBOARD
} AppState;

// UI Components
typedef struct {
    int x, y, w, h;
    char text[64];
    int is_password;
    int active;
} InputField;

typedef struct {
    int x, y, w, h;
    char text[32];
} Button;

// Global State
Display *dpy;
int screen;
Window win;
int win_width = DEFAULT_WIDTH;
int win_height = DEFAULT_HEIGHT;
XftDraw *xft_draw;
XftColor color_fg, color_bg, color_input_bg, color_input_text;
XftFont *font_large, *font_normal, *font_small;
AppState current_state = STATE_LOGIN;
int sock = -1;
char current_user[32];
char status_message[256];
double current_balance = 0.0;

// Inputs
InputField input_user = {0, 0, 300, 30, "", 0, 0};
InputField input_pass = {0, 0, 300, 30, "", 1, 0};
InputField input_target = {0, 0, 300, 30, "", 0, 0};
InputField input_amount = {0, 0, 300, 30, "", 0, 0};

// Buttons
Button btn_login = {0, 0, 140, 40, "LOGIN"};
Button btn_register = {0, 0, 140, 40, "REGISTER"};
Button btn_transfer = {0, 0, 300, 40, "REDISTRIBUTE WEALTH"};
Button btn_refresh = {50, 50, 100, 30, "REFRESH"};
Button btn_logout = {650, 50, 100, 30, "LOGOUT"};

void update_layout() {
    int cx = win_width / 2;
    int cy = win_height / 2;

    // Login State Layout
    input_user.x = cx - 150; input_user.y = cy - 80;
    input_pass.x = cx - 150; input_pass.y = cy - 20;
    btn_login.x = cx - 150; btn_login.y = cy + 40;
    btn_register.x = cx + 10; btn_register.y = cy + 40;

    // Dashboard State Layout
    input_target.x = cx - 150; input_target.y = cy - 20;
    input_amount.x = cx - 150; input_amount.y = cy + 40;
    btn_transfer.x = cx - 150; btn_transfer.y = cy + 100;
    
    // Fixed / Relative Positions
    btn_refresh.x = 50; btn_refresh.y = 50;
    btn_logout.x = win_width - 150; btn_logout.y = 50;
}

void init_x() {
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    screen = DefaultScreen(dpy);
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 10, 10, win_width, win_height, 1,
                              BlackPixel(dpy, screen), WhitePixel(dpy, screen));
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    XStoreName(dpy, win, "MyPaySU - The People's Wallet");
    XMapWindow(dpy, win);

    // Initialize Xft
    xft_draw = XftDrawCreate(dpy, win, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen));
    
    // Load Colors
    XftColorAllocName(dpy, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen), COLOR_FG, &color_fg);
    XftColorAllocName(dpy, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen), COLOR_BG, &color_bg);
    XftColorAllocName(dpy, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen), COLOR_INPUT, &color_input_bg);
    XftColorAllocName(dpy, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen), COLOR_TEXT_INPUT, &color_input_text);

    // Load Fonts
    font_large = XftFontOpenName(dpy, screen, "Liberation Sans:size=24:bold");
    font_normal = XftFontOpenName(dpy, screen, "Liberation Sans:size=14");
    font_small = XftFontOpenName(dpy, screen, "Liberation Sans:size=10");

    if (!font_large || !font_normal) {
        fprintf(stderr, "Could not load fonts. Ensure Liberation Sans is installed.\n");
        exit(1);
    }
}

void connect_server() {
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        snprintf(status_message, sizeof(status_message), "Socket creation error");
        return;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        snprintf(status_message, sizeof(status_message), "Invalid address");
        return;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        snprintf(status_message, sizeof(status_message), "Connection Failed. Is Server Running?");
        return;
    }
}

void send_request(SovietMessage *req, SovietMessage *res) {
    if (sock < 0) connect_server();
    if (sock < 0) return;

    send(sock, req, sizeof(SovietMessage), 0);
    recv(sock, res, sizeof(SovietMessage), 0);
    
    strncpy(status_message, res->message, sizeof(status_message) - 1);
}

void draw_rect(int x, int y, int w, int h, XftColor color) {
    XftDrawRect(xft_draw, &color, x, y, w, h);
}

void draw_text(int x, int y, const char *text, XftFont *font, XftColor color) {
    XftDrawStringUtf8(xft_draw, &color, font, x, y + font->ascent, (FcChar8 *)text, strlen(text));
}

void draw_button(Button b) {
    draw_rect(b.x, b.y, b.w, b.h, color_fg); // Gold background
    draw_rect(b.x + 2, b.y + 2, b.w - 4, b.h - 4, color_bg); // Red inner
    
    // Center text
    XGlyphInfo extents;
    XftTextExtentsUtf8(dpy, font_normal, (FcChar8 *)b.text, strlen(b.text), &extents);
    int tx = b.x + (b.w - extents.width) / 2;
    int ty = b.y + (b.h - extents.height) / 2; // Approximate centering
    draw_text(tx, ty, b.text, font_normal, color_fg);
}

void draw_input(InputField f) {
    draw_rect(f.x, f.y, f.w, f.h, color_input_bg);
    if (f.active) {
        draw_rect(f.x - 2, f.y - 2, f.w + 4, f.h + 4, color_fg); // Highlight
        draw_rect(f.x, f.y, f.w, f.h, color_input_bg);
    }
    
    char display_text[64];
    if (f.is_password) {
        memset(display_text, '*', strlen(f.text));
        display_text[strlen(f.text)] = '\0';
    } else {
        strcpy(display_text, f.text);
    }
    draw_text(f.x + 5, f.y + 5, display_text, font_normal, color_input_text);
}

void render() {
    // Clear screen (Red background)
    draw_rect(0, 0, win_width, win_height, color_bg);

    // Header
    const char *title = "MyPaySU";
    XGlyphInfo extents;
    XftTextExtentsUtf8(dpy, font_large, (FcChar8 *)title, strlen(title), &extents);
    draw_text((win_width - extents.width) / 2, 50, title, font_large, color_fg);

    // Subheader
    const char *subtitle = "The Official Payment System of the Proletariat";
    XftTextExtentsUtf8(dpy, font_small, (FcChar8 *)subtitle, strlen(subtitle), &extents);
    draw_text((win_width - extents.width) / 2, 90, subtitle, font_small, color_fg);

    // Status Message
    draw_text(10, win_height - 30, status_message, font_small, color_fg);

    if (current_state == STATE_LOGIN) {
        draw_text(input_user.x, input_user.y - 20, "Username:", font_small, color_fg);
        draw_input(input_user);
        draw_text(input_pass.x, input_pass.y - 20, "Password:", font_small, color_fg);
        draw_input(input_pass);
        
        draw_button(btn_login);
        draw_button(btn_register);
    } else if (current_state == STATE_DASHBOARD) {
        char bal_str[64];
        snprintf(bal_str, sizeof(bal_str), "Balance: %.2f SXMR", current_balance);
        draw_text(50, 150, bal_str, font_large, color_fg);

        draw_text(input_target.x, input_target.y - 20, "Recipient Comrade:", font_small, color_fg);
        draw_input(input_target);
        
        draw_text(input_amount.x, input_amount.y - 20, "Amount (SXMR):", font_small, color_fg);
        draw_input(input_amount);
        
        draw_button(btn_transfer);
        draw_button(btn_refresh);
        draw_button(btn_logout);
    }
}

int is_inside(int x, int y, Button b) {
    return x >= b.x && x <= b.x + b.w && y >= b.y && y <= b.y + b.h;
}

int is_inside_input(int x, int y, InputField f) {
    return x >= f.x && x <= f.x + f.w && y >= f.y && y <= f.y + f.h;
}

void handle_click(int x, int y) {
    if (current_state == STATE_LOGIN) {
        if (is_inside(x, y, btn_login)) {
            SovietMessage req, res;
            req.type = REQ_LOGIN;
            strcpy(req.username, input_user.text);
            strcpy(req.password, input_pass.text);
            send_request(&req, &res);
            if (res.type == RES_OK) {
                current_state = STATE_DASHBOARD;
                strcpy(current_user, input_user.text);
                // Fetch balance
                req.type = REQ_BALANCE;
                send_request(&req, &res);
                if (res.type == RES_OK) current_balance = res.amount;
            }
        } else if (is_inside(x, y, btn_register)) {
            SovietMessage req, res;
            req.type = REQ_REGISTER;
            strcpy(req.username, input_user.text);
            strcpy(req.password, input_pass.text);
            send_request(&req, &res);
        }
        
        // Focus inputs
        input_user.active = is_inside_input(x, y, input_user);
        input_pass.active = is_inside_input(x, y, input_pass);

    } else if (current_state == STATE_DASHBOARD) {
        if (is_inside(x, y, btn_transfer)) {
            SovietMessage req, res;
            req.type = REQ_TRANSFER;
            strcpy(req.username, current_user);
            strcpy(req.target_user, input_target.text);
            req.amount = atof(input_amount.text);
            send_request(&req, &res);
            
            // Refresh balance
            req.type = REQ_BALANCE;
            send_request(&req, &res);
            if (res.type == RES_OK) current_balance = res.amount;

        } else if (is_inside(x, y, btn_refresh)) {
            SovietMessage req, res;
            req.type = REQ_BALANCE;
            strcpy(req.username, current_user);
            send_request(&req, &res);
            if (res.type == RES_OK) current_balance = res.amount;
        } else if (is_inside(x, y, btn_logout)) {
            current_state = STATE_LOGIN;
            input_pass.text[0] = '\0'; // clear pass
            input_user.text[0] = '\0'; // clear user
        }

        input_target.active = is_inside_input(x, y, input_target);
        input_amount.active = is_inside_input(x, y, input_amount);
    }
}

void handle_keypress(KeySym key, char *buf) {
    InputField *active = NULL;
    if (current_state == STATE_LOGIN) {
        if (input_user.active) active = &input_user;
        if (input_pass.active) active = &input_pass;
    } else {
        if (input_target.active) active = &input_target;
        if (input_amount.active) active = &input_amount;
    }

    if (active) {
        if (key == XK_BackSpace) {
            int len = strlen(active->text);
            if (len > 0) active->text[len - 1] = '\0';
        } else if (strlen(active->text) < sizeof(active->text) - 1 && buf[0] >= 32 && buf[0] <= 126) {
            strcat(active->text, buf);
        }
    }
}

int main() {
    init_x();
    update_layout();

    XEvent e;
    while (1) {
        XNextEvent(dpy, &e);
        if (e.type == ConfigureNotify) {
            win_width = e.xconfigure.width;
            win_height = e.xconfigure.height;
            update_layout();
            render(); // Redraw on resize
        } else if (e.type == Expose) {
            render();
        } else if (e.type == ButtonPress) {
            if (e.xbutton.button == 1) { // Left click
                handle_click(e.xbutton.x, e.xbutton.y);
                render();
            }
        } else if (e.type == KeyPress) {
            char buf[32];
            KeySym key;
            XLookupString(&e.xkey, buf, sizeof(buf), &key, NULL);
            handle_keypress(key, buf);
            render();
        }
    }

    XCloseDisplay(dpy);
    return 0;
}
