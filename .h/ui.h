#pragma once
#include <vector>
#include <windows.h>
#include <memory>
#include <cstring>

class Mouse{
    public:
        SHORT x;
        SHORT y;
        int button;

        Mouse(SHORT _x, SHORT _y, int _button):
            x(_x),
            y(_y),
            button(_button)
        {}
};

class Element{
    
    public:
        SHORT col = 0;
        SHORT row = 0;
        SHORT width = 0;
        SHORT height = 0;
        bool render = true;

        Element* father;
        std::vector<std::unique_ptr<Element>> son;

        std::vector<CHAR_INFO> content;
        WORD format = 0x07;

        Element(SHORT _col, SHORT _row, SHORT _width, SHORT _height):
            col(_col),
            row(_row),
            width(_width),
            height(_height),
            content((int)_width * _height, CHAR_INFO{L' ', 0x07})
        {}

        CHAR_INFO* At(SHORT _col, SHORT _row){
            if(_col < 0 || _row < 0 || _col >= width || _row >= height){
                printf("AT OOB: col=%d, row=%d\n", _col, _row);
                return nullptr;
            }
            return &content[(int)_row * width + _col];
        }

        virtual ~Element() = default;
        virtual void Render(Element* target) = 0;
        void Build(){
            for(int i = 0; i < son.size(); i++){
                Element* _son = son[i].get();
                if(_son->render) _son->Render(this);
            }
        }

        bool visible(SHORT _col, SHORT _row){
            return col <= _col && _col < col + width && row <= _row && _row < row + height;
        }
        Element* hittest(SHORT _col, SHORT _row){
            for(int i = son.size() - 1; i >= 0; i--){
                if(son[i].get()->visible(_col - col, _row - row)){
                    return son[i].get()->hittest(_col - col, _row - row);
                }
            }
            return this;
        }

        virtual bool OnHover() {return false;};
        virtual bool Leave() {return false;};
};


void InitConsole(const wchar_t* title){
    system("cls");
    HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);

    SetConsoleTitleW(title);

    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(h_out, &ci);
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(h_out, &ci);

    DWORD mode;
    GetConsoleMode(h_in, &mode);
    mode |= ENABLE_MOUSE_INPUT;         
    mode &= ~ENABLE_QUICK_EDIT_MODE;      
    mode |= ENABLE_EXTENDED_FLAGS;       
    SetConsoleMode(h_in, mode);
}