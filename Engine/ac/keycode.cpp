
#include "ac/keycode.h"

#include <allegro.h>

int GetKeyForKeyPressCb(int keycode)
{
    // lower case 'a'..'z' do not exist as keycodes, only ascii. 'A'..'Z' do though!
    return (keycode >= 'a' && keycode <= 'z') ? keycode - 32 : keycode;
}

int PlatformKeyFromAgsKey(int key)
{
    int platformKey = -1;

    switch (key) {
        // ctrl-[A-Z] keys are numbered 1-26 for A-Z
        case eAGSKeyCodeCtrlA: platformKey = 1; break;
        case eAGSKeyCodeCtrlB: platformKey = 2; break;
        case eAGSKeyCodeCtrlC: platformKey = 3; break;
        case eAGSKeyCodeCtrlD: platformKey = 4; break;
        case eAGSKeyCodeCtrlE: platformKey = 5; break;
        case eAGSKeyCodeCtrlF: platformKey = 6; break;
        case eAGSKeyCodeCtrlG: platformKey = 7; break;
        // case eAGSKeyCodeCtrlH: // overlap with backspace
        // case eAGSKeyCodeCtrlI: // overlap with tab
        case eAGSKeyCodeCtrlJ: platformKey = 10; break;
        case eAGSKeyCodeCtrlK: platformKey = 11; break;
        case eAGSKeyCodeCtrlL: platformKey = 12; break;
        // case eAGSKeyCodeCtrlM: // overlap with return
        case eAGSKeyCodeCtrlN: platformKey = 14; break;
        case eAGSKeyCodeCtrlO: platformKey = 15; break;
        case eAGSKeyCodeCtrlP: platformKey = 16; break;
        case eAGSKeyCodeCtrlQ: platformKey = 17; break;
        case eAGSKeyCodeCtrlR: platformKey = 18; break;
        case eAGSKeyCodeCtrlS: platformKey = 19; break;
        case eAGSKeyCodeCtrlT: platformKey = 20; break;
        case eAGSKeyCodeCtrlU: platformKey = 21; break;
        case eAGSKeyCodeCtrlV: platformKey = 22; break;
        case eAGSKeyCodeCtrlW: platformKey = 23; break;
        case eAGSKeyCodeCtrlX: platformKey = 24; break;
        case eAGSKeyCodeCtrlY: platformKey = 25; break;
        case eAGSKeyCodeCtrlZ: platformKey = 26; break;

        case eAGSKeyCodeBackspace: platformKey = (__allegro_KEY_BACKSPACE << 8) | 8; break;
        case eAGSKeyCodeTab: platformKey = (__allegro_KEY_TAB << 8) | 9; break;
        case eAGSKeyCodeReturn: platformKey = (__allegro_KEY_ENTER << 8) | 13; break;
        case eAGSKeyCodeEscape: platformKey = (__allegro_KEY_ESC << 8) | 27; break;
        case eAGSKeyCodeSpace: platformKey = (__allegro_KEY_SPACE << 8) | ' '; break;
        case eAGSKeyCodeExclamationMark: platformKey = '!'; break;
        case eAGSKeyCodeDoubleQuote: platformKey = '"'; break;
        case eAGSKeyCodeHash: platformKey = '#'; break;
        case eAGSKeyCodeDollar: platformKey = '$'; break;
        case eAGSKeyCodePercent: platformKey = '%'; break;
        case eAGSKeyCodeAmpersand: platformKey = '&'; break;
        case eAGSKeyCodeSingleQuote: platformKey = '\''; break;
        case eAGSKeyCodeOpenParenthesis: platformKey = '('; break;
        case eAGSKeyCodeCloseParenthesis: platformKey = ')'; break;
        case eAGSKeyCodeAsterisk: platformKey = '*'; break;
        case eAGSKeyCodePlus: platformKey = '+'; break;
        case eAGSKeyCodeComma: platformKey = ','; break;
        case eAGSKeyCodeHyphen: platformKey = '-'; break;
        case eAGSKeyCodePeriod: platformKey = '.'; break;
        case eAGSKeyCodeForwardSlash: platformKey = '/'; break;
        case eAGSKeyCodeColon: platformKey = ':'; break;
        case eAGSKeyCodeSemiColon: platformKey = ';'; break;
        case eAGSKeyCodeLessThan: platformKey = '<'; break;
        case eAGSKeyCodeEquals: platformKey = '='; break;
        case eAGSKeyCodeGreaterThan: platformKey = '>'; break;
        case eAGSKeyCodeQuestionMark: platformKey = '?'; break;
        case eAGSKeyCodeAt: platformKey = '@'; break;
        case eAGSKeyCodeOpenBracket: platformKey = '['; break;
        case eAGSKeyCodeBackSlash: platformKey = '\\'; break;
        case eAGSKeyCodeCloseBracket: platformKey = ']'; break;
        case eAGSKeyCodeUnderscore: platformKey = '_'; break;

        case eAGSKeyCode0: platformKey = (__allegro_KEY_0 << 8) | '0'; break;
        case eAGSKeyCode1: platformKey = (__allegro_KEY_1 << 8) | '1'; break;
        case eAGSKeyCode2: platformKey = (__allegro_KEY_2 << 8) | '2'; break;
        case eAGSKeyCode3: platformKey = (__allegro_KEY_3 << 8) | '3'; break;
        case eAGSKeyCode4: platformKey = (__allegro_KEY_4 << 8) | '4'; break;
        case eAGSKeyCode5: platformKey = (__allegro_KEY_5 << 8) | '5'; break;
        case eAGSKeyCode6: platformKey = (__allegro_KEY_6 << 8) | '6'; break;
        case eAGSKeyCode7: platformKey = (__allegro_KEY_7 << 8) | '7'; break;
        case eAGSKeyCode8: platformKey = (__allegro_KEY_8 << 8) | '8'; break;
        case eAGSKeyCode9: platformKey = (__allegro_KEY_9 << 8) | '9'; break;

        case eAGSKeyCodeA: platformKey = (__allegro_KEY_A << 8) | 'a'; break;
        case eAGSKeyCodeB: platformKey = (__allegro_KEY_B << 8) | 'b'; break;
        case eAGSKeyCodeC: platformKey = (__allegro_KEY_C << 8) | 'c'; break;
        case eAGSKeyCodeD: platformKey = (__allegro_KEY_D << 8) | 'd'; break;
        case eAGSKeyCodeE: platformKey = (__allegro_KEY_E << 8) | 'e'; break;
        case eAGSKeyCodeF: platformKey = (__allegro_KEY_F << 8) | 'f'; break;
        case eAGSKeyCodeG: platformKey = (__allegro_KEY_G << 8) | 'g'; break;
        case eAGSKeyCodeH: platformKey = (__allegro_KEY_H << 8) | 'h'; break;
        case eAGSKeyCodeI: platformKey = (__allegro_KEY_I << 8) | 'i'; break;
        case eAGSKeyCodeJ: platformKey = (__allegro_KEY_J << 8) | 'j'; break;
        case eAGSKeyCodeK: platformKey = (__allegro_KEY_K << 8) | 'k'; break;
        case eAGSKeyCodeL: platformKey = (__allegro_KEY_L << 8) | 'l'; break;
        case eAGSKeyCodeM: platformKey = (__allegro_KEY_M << 8) | 'm'; break;
        case eAGSKeyCodeN: platformKey = (__allegro_KEY_N << 8) | 'n'; break;
        case eAGSKeyCodeO: platformKey = (__allegro_KEY_O << 8) | 'o'; break;
        case eAGSKeyCodeP: platformKey = (__allegro_KEY_P << 8) | 'p'; break;
        case eAGSKeyCodeQ: platformKey = (__allegro_KEY_Q << 8) | 'q'; break;
        case eAGSKeyCodeR: platformKey = (__allegro_KEY_R << 8) | 'r'; break;
        case eAGSKeyCodeS: platformKey = (__allegro_KEY_S << 8) | 's'; break;
        case eAGSKeyCodeT: platformKey = (__allegro_KEY_T << 8) | 't'; break;
        case eAGSKeyCodeU: platformKey = (__allegro_KEY_U << 8) | 'u'; break;
        case eAGSKeyCodeV: platformKey = (__allegro_KEY_V << 8) | 'v'; break;
        case eAGSKeyCodeW: platformKey = (__allegro_KEY_W << 8) | 'w'; break;
        case eAGSKeyCodeX: platformKey = (__allegro_KEY_X << 8) | 'x'; break;
        case eAGSKeyCodeY: platformKey = (__allegro_KEY_Y << 8) | 'y'; break;
        case eAGSKeyCodeZ: platformKey = (__allegro_KEY_Z << 8) | 'z'; break;

        case eAGSKeyCodeF1: platformKey = __allegro_KEY_F1 << 8; break;
        case eAGSKeyCodeF2: platformKey = __allegro_KEY_F2 << 8; break;
        case eAGSKeyCodeF3: platformKey = __allegro_KEY_F3 << 8; break;
        case eAGSKeyCodeF4: platformKey = __allegro_KEY_F4 << 8; break;
        case eAGSKeyCodeF5: platformKey = __allegro_KEY_F5 << 8; break;
        case eAGSKeyCodeF6: platformKey = __allegro_KEY_F6 << 8; break;
        case eAGSKeyCodeF7: platformKey = __allegro_KEY_F7 << 8; break;
        case eAGSKeyCodeF8: platformKey = __allegro_KEY_F8 << 8; break;
        case eAGSKeyCodeF9: platformKey = __allegro_KEY_F9 << 8; break;
        case eAGSKeyCodeF10: platformKey = __allegro_KEY_F10 << 8; break;
        case eAGSKeyCodeF11: platformKey = __allegro_KEY_F11 << 8; break;
        case eAGSKeyCodeF12: platformKey = __allegro_KEY_F12 << 8; break;

        case eAGSKeyCodeHome: platformKey = __allegro_KEY_HOME << 8; break;
        case eAGSKeyCodeUpArrow: platformKey = __allegro_KEY_UP << 8; break;
        case eAGSKeyCodePageUp: platformKey = __allegro_KEY_PGUP << 8; break;
        case eAGSKeyCodeLeftArrow: platformKey = __allegro_KEY_LEFT << 8; break;
        case eAGSKeyCodeNumPad5: platformKey = __allegro_KEY_5_PAD << 8; break;
        case eAGSKeyCodeRightArrow: platformKey = __allegro_KEY_RIGHT << 8; break;
        case eAGSKeyCodeEnd: platformKey = __allegro_KEY_END << 8; break;
        case eAGSKeyCodeDownArrow: platformKey = __allegro_KEY_DOWN << 8; break;
        case eAGSKeyCodePageDown: platformKey = __allegro_KEY_PGDN << 8; break;
        case eAGSKeyCodeInsert: platformKey = __allegro_KEY_INSERT << 8; break;
        case eAGSKeyCodeDelete: platformKey = __allegro_KEY_DEL << 8; break;
    }

    return platformKey;
}

SDL_Keysym SDLKeysymFromAgsKey (int key)
{
    SDL_Keysym sk;
    sk.scancode = SDL_SCANCODE_UNKNOWN;
    sk.sym = SDLK_UNKNOWN;

    switch (key) {
        // ctrl-[A-Z] keys are numbered 1-26 for A-Z
        case eAGSKeyCodeCtrlA: { sk.scancode = SDL_SCANCODE_A; sk.sym = SDLK_a; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlB: { sk.scancode = SDL_SCANCODE_B; sk.sym = SDLK_b; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlC: { sk.scancode = SDL_SCANCODE_C; sk.sym = SDLK_c; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlD: { sk.scancode = SDL_SCANCODE_D; sk.sym = SDLK_d; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlE: { sk.scancode = SDL_SCANCODE_E; sk.sym = SDLK_e; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlF: { sk.scancode = SDL_SCANCODE_F; sk.sym = SDLK_f; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlG: { sk.scancode = SDL_SCANCODE_G; sk.sym = SDLK_g; sk.mod = KMOD_CTRL; } break;
            // case eAGSKeyCodeCtrlH: // overlap with backspace
            // case eAGSKeyCodeCtrlI: // overlap with tab
        case eAGSKeyCodeCtrlJ: { sk.scancode = SDL_SCANCODE_J; sk.sym = SDLK_j; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlK: { sk.scancode = SDL_SCANCODE_K; sk.sym = SDLK_k; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlL: { sk.scancode = SDL_SCANCODE_L; sk.sym = SDLK_l; sk.mod = KMOD_CTRL; } break;
            // case eAGSKeyCodeCtrlM: // overlap with return
        case eAGSKeyCodeCtrlN: { sk.scancode = SDL_SCANCODE_N; sk.sym = SDLK_n; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlO: { sk.scancode = SDL_SCANCODE_O; sk.sym = SDLK_o; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlP: { sk.scancode = SDL_SCANCODE_P; sk.sym = SDLK_p; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlQ: { sk.scancode = SDL_SCANCODE_Q; sk.sym = SDLK_q; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlR: { sk.scancode = SDL_SCANCODE_R; sk.sym = SDLK_r; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlS: { sk.scancode = SDL_SCANCODE_S; sk.sym = SDLK_s; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlT: { sk.scancode = SDL_SCANCODE_T; sk.sym = SDLK_t; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlU: { sk.scancode = SDL_SCANCODE_U; sk.sym = SDLK_u; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlV: { sk.scancode = SDL_SCANCODE_V; sk.sym = SDLK_v; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlW: { sk.scancode = SDL_SCANCODE_W; sk.sym = SDLK_w; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlX: { sk.scancode = SDL_SCANCODE_X; sk.sym = SDLK_x; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlY: { sk.scancode = SDL_SCANCODE_Y; sk.sym = SDLK_y; sk.mod = KMOD_CTRL; } break;
        case eAGSKeyCodeCtrlZ: { sk.scancode = SDL_SCANCODE_Z; sk.sym = SDLK_z; sk.mod = KMOD_CTRL; } break;

        case eAGSKeyCodeBackspace: { sk.scancode = SDL_SCANCODE_BACKSPACE; sk.sym = SDLK_BACKSPACE; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeTab: { sk.scancode = SDL_SCANCODE_TAB; sk.sym = SDLK_TAB; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeReturn: { sk.scancode = SDL_SCANCODE_RETURN; sk.sym = SDLK_RETURN; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeEscape: { sk.scancode = SDL_SCANCODE_ESCAPE; sk.sym = SDLK_ESCAPE; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeSpace: { sk.scancode = SDL_SCANCODE_SPACE; sk.sym = SDLK_SPACE; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeExclamationMark: { sk.scancode = SDL_SCANCODE_KP_EXCLAM; sk.sym = SDLK_KP_EXCLAM; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeDoubleQuote: { sk.sym = SDLK_QUOTEDBL; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeHash: { sk.sym = SDLK_HASH; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeDollar: { sk.sym = SDLK_DOLLAR; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodePercent: { sk.sym = SDLK_PERCENT; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeAmpersand: { sk.sym = SDLK_AMPERSAND; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeSingleQuote: { sk.scancode = SDL_SCANCODE_APOSTROPHE; sk.sym = SDLK_QUOTE; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeOpenParenthesis: { sk.sym = SDLK_LEFTPAREN; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeCloseParenthesis: { sk.sym = SDLK_RIGHTPAREN; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeAsterisk: { sk.sym = SDLK_ASTERISK; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodePlus: { sk.sym = SDLK_PLUS; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeComma: { sk.scancode = SDL_SCANCODE_COMMA; sk.sym = SDLK_COMMA; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeHyphen: { sk.scancode = SDL_SCANCODE_MINUS; sk.sym = SDLK_MINUS; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodePeriod: { sk.scancode = SDL_SCANCODE_PERIOD; sk.sym = SDLK_PERIOD; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeForwardSlash: { sk.scancode = SDL_SCANCODE_SLASH; sk.sym = SDLK_SLASH; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeColon: { sk.sym = SDLK_COLON; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeSemiColon: { sk.scancode = SDL_SCANCODE_SEMICOLON; sk.sym = SDLK_SEMICOLON; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeLessThan: { sk.sym = SDLK_LESS; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeEquals: { sk.scancode = SDL_SCANCODE_EQUALS; sk.sym = SDLK_EQUALS; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeGreaterThan: { sk.sym = SDLK_GREATER; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeQuestionMark: { sk.sym = SDLK_QUESTION; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeAt: { sk.sym = SDLK_AT; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeOpenBracket: { sk.scancode = SDL_SCANCODE_LEFTBRACKET; sk.sym = SDLK_LEFTBRACKET; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeBackSlash: { sk.scancode = SDL_SCANCODE_BACKSLASH; sk.sym = SDLK_BACKSLASH; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeCloseBracket: { sk.scancode = SDL_SCANCODE_RIGHTBRACKET; sk.sym = SDLK_RIGHTBRACKET; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeUnderscore: { sk.sym = SDLK_UNDERSCORE; sk.mod = KMOD_NONE; } break;

        case eAGSKeyCode0: { sk.scancode = SDL_SCANCODE_0; sk.sym = SDLK_0; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode1: { sk.scancode = SDL_SCANCODE_1; sk.sym = SDLK_1; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode2: { sk.scancode = SDL_SCANCODE_2; sk.sym = SDLK_2; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode3: { sk.scancode = SDL_SCANCODE_3; sk.sym = SDLK_3; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode4: { sk.scancode = SDL_SCANCODE_4; sk.sym = SDLK_4; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode5: { sk.scancode = SDL_SCANCODE_5; sk.sym = SDLK_5; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode6: { sk.scancode = SDL_SCANCODE_6; sk.sym = SDLK_6; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode7: { sk.scancode = SDL_SCANCODE_7; sk.sym = SDLK_7; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode8: { sk.scancode = SDL_SCANCODE_8; sk.sym = SDLK_8; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCode9: { sk.scancode = SDL_SCANCODE_9; sk.sym = SDLK_9; sk.mod = KMOD_NONE; } break;

        case eAGSKeyCodeA: { sk.scancode = SDL_SCANCODE_A; sk.sym = SDLK_a; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeB: { sk.scancode = SDL_SCANCODE_B; sk.sym = SDLK_b; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeC: { sk.scancode = SDL_SCANCODE_C; sk.sym = SDLK_c; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeD: { sk.scancode = SDL_SCANCODE_D; sk.sym = SDLK_d; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeE: { sk.scancode = SDL_SCANCODE_E; sk.sym = SDLK_e; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF: { sk.scancode = SDL_SCANCODE_F; sk.sym = SDLK_f; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeG: { sk.scancode = SDL_SCANCODE_G; sk.sym = SDLK_g; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeH: { sk.scancode = SDL_SCANCODE_H; sk.sym = SDLK_h; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeI: { sk.scancode = SDL_SCANCODE_I; sk.sym = SDLK_i; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeJ: { sk.scancode = SDL_SCANCODE_J; sk.sym = SDLK_j; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeK: { sk.scancode = SDL_SCANCODE_K; sk.sym = SDLK_k; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeL: { sk.scancode = SDL_SCANCODE_L; sk.sym = SDLK_l; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeM: { sk.scancode = SDL_SCANCODE_M; sk.sym = SDLK_m; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeN: { sk.scancode = SDL_SCANCODE_N; sk.sym = SDLK_n; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeO: { sk.scancode = SDL_SCANCODE_O; sk.sym = SDLK_o; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeP: { sk.scancode = SDL_SCANCODE_P; sk.sym = SDLK_p; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeQ: { sk.scancode = SDL_SCANCODE_Q; sk.sym = SDLK_q; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeR: { sk.scancode = SDL_SCANCODE_R; sk.sym = SDLK_r; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeS: { sk.scancode = SDL_SCANCODE_S; sk.sym = SDLK_s; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeT: { sk.scancode = SDL_SCANCODE_T; sk.sym = SDLK_t; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeU: { sk.scancode = SDL_SCANCODE_U; sk.sym = SDLK_u; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeV: { sk.scancode = SDL_SCANCODE_V; sk.sym = SDLK_v; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeW: { sk.scancode = SDL_SCANCODE_W; sk.sym = SDLK_w; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeX: { sk.scancode = SDL_SCANCODE_X; sk.sym = SDLK_x; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeY: { sk.scancode = SDL_SCANCODE_Y; sk.sym = SDLK_y; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeZ: { sk.scancode = SDL_SCANCODE_Z; sk.sym = SDLK_z; sk.mod = KMOD_NONE; } break;

        case eAGSKeyCodeF1: { sk.scancode = SDL_SCANCODE_F1; sk.sym = SDLK_F1; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF2: { sk.scancode = SDL_SCANCODE_F2; sk.sym = SDLK_F2; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF3: { sk.scancode = SDL_SCANCODE_F3; sk.sym = SDLK_F3; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF4: { sk.scancode = SDL_SCANCODE_F4; sk.sym = SDLK_F4; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF5: { sk.scancode = SDL_SCANCODE_F5; sk.sym = SDLK_F5; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF6: { sk.scancode = SDL_SCANCODE_F6; sk.sym = SDLK_F6; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF7: { sk.scancode = SDL_SCANCODE_F7; sk.sym = SDLK_F7; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF8: { sk.scancode = SDL_SCANCODE_F8; sk.sym = SDLK_F8; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF9: { sk.scancode = SDL_SCANCODE_F9; sk.sym = SDLK_F9; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF10: { sk.scancode = SDL_SCANCODE_F10; sk.sym = SDLK_F10; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF11: { sk.scancode = SDL_SCANCODE_F11; sk.sym = SDLK_F11; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeF12: { sk.scancode = SDL_SCANCODE_F12; sk.sym = SDLK_F12; sk.mod = KMOD_NONE; } break;

        case eAGSKeyCodeHome: { sk.scancode = SDL_SCANCODE_HOME; sk.sym = SDLK_HOME; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeUpArrow: { sk.scancode = SDL_SCANCODE_UP; sk.sym = SDLK_UP; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodePageUp: { sk.scancode = SDL_SCANCODE_PAGEUP; sk.sym = SDLK_PAGEUP; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeLeftArrow: { sk.scancode = SDL_SCANCODE_LEFT; sk.sym = SDLK_LEFT; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeNumPad5: { sk.scancode = SDL_SCANCODE_KP_5; sk.sym = SDLK_KP_5; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeRightArrow: { sk.scancode = SDL_SCANCODE_RIGHT; sk.sym = SDLK_RIGHT; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeEnd: { sk.scancode = SDL_SCANCODE_END; sk.sym = SDLK_END; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeDownArrow: { sk.scancode = SDL_SCANCODE_DOWN; sk.sym = SDLK_DOWN; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodePageDown: { sk.scancode = SDL_SCANCODE_PAGEDOWN; sk.sym = SDLK_PAGEDOWN; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeInsert: { sk.scancode = SDL_SCANCODE_INSERT; sk.sym = SDLK_INSERT; sk.mod = KMOD_NONE; } break;
        case eAGSKeyCodeDelete: { sk.scancode = SDL_SCANCODE_DELETE; sk.sym = SDLK_DELETE; sk.mod = KMOD_NONE; } break;
    }
    return sk;
}
