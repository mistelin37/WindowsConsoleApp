#pragma once
#include "ui.h"
#include "Button.h"

class Window: public Element{
    public:

        bool border[4] = {false,false,false,false};//右上左下
        CHAR_INFO border_edge = CHAR_INFO{L' ', 0x70};
        CHAR_INFO border_corner = CHAR_INFO{L' ', 0x70};

        //构建方法，传入左上角列坐标，行坐标，宽和高
        Window(SHORT _col, SHORT _row, SHORT _width, SHORT _height, std::string _title = ""):
            Element(_col, _row, _width, _height)
        {
            if(!_title.empty()){
                Button* drg_btn = this->CreateSon<Button>(0, 0, _width, 1);
                drg_btn->SetFormat(0x70,0x70,0x70,0x70);
                drg_btn->label->aligment = 0;
                drg_btn->label->content = "  " + _title;
                drg_btn->on_click = [this](Mouse event){
                    start_col = col;
                    start_row = row;
                    start_x = event.x;
                    start_y = event.y;
                };
                drg_btn->hold = [this](Mouse event){
                    col = start_col + event.x - start_x;
                    row = start_row + event.y - start_y;
                };
            }
        }

        //设置是否有边框
        void SetBorder(bool _border0, bool _border1, bool _border2, bool _border3){
            border[0] = _border0;
            border[1] = _border1;
            border[2] = _border2;
            border[3] = _border3;
        }
        //设置边框样式
        void SetBorderStyle(CHAR_INFO _border_edge, 
                            CHAR_INFO _border_corner){
            border_edge = _border_edge;
            border_corner = _border_corner;
        }
        //设置背景样式
        void SetBg(WORD _format){
            format = _format;

        }
        
        //将内容物写入到控制台的左上角
        void FlushAll(){
            content.assign((int)width * height, CHAR_INFO{L' ', 0x07});
            Render(nullptr);

            if (width <= 0 || height <= 0) return;
            HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD buf_size = {width, height};
            COORD buf_coord = {0, 0};
            SMALL_RECT region = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};

            WriteConsoleOutputW(h_out, content.data(), buf_size, buf_coord, &region);
        }

        void Render(Element* target){
            FlushBg();
            Build();
            FlushBorder();

            if(target != nullptr){
                for(int i = 0; i < width; i++){
                    for(int j = 0; j < height; j++){
                        if(0 <= col + i && col + i < target->width && 0 <= row + j && row + j < target->height)
                        *target->At(col + i, row + j) = *this->At(i, j);
                    }
                }
            }
        }

        //创建一个子控件
        template <typename T, typename... Args>
        T* CreateSon(Args&&... args){
            auto u_ptr = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = u_ptr.get();
            ptr->father = this;
            son.push_back(std::move(u_ptr));
            return ptr;
        }
        //摧毁一个子控件
        void DestorySon(int _id){
            son.erase(son.begin() + _id);
        }

    private:
        //自带控件的中间变量
        SHORT start_col = 0;
        SHORT start_row = 0;
        SHORT start_x = 0;
        SHORT start_y = 0;

        //写入背景
        void FlushBg(){
            for(int i = 0; i < width * height; i++){
                content[i].Attributes = format;
            }
        }
        //写入边框
        void FlushBorder(){
            if(border[0]){
                for(int i = 1; i < height - 1; i++){
                    *this->At(width - 1, i) = border_edge;
                    *this->At(width - 2, i) = border_edge;
                }
                *this->At(width - 1, 0) = border_corner;
                *this->At(width - 2, 0) = border_corner;
                *this->At(width - 1, height - 1) = border_corner;
                *this->At(width - 2, height - 1) = border_corner;
            }
            if(border[1]){
                for(int i = 1; i < width - 1; i++){
                    *this->At(i,0) = border_edge;
                }
                *this->At(0,0) = border_corner;
                *this->At(1,0) = border_corner;
                *this->At(width - 1,0) = border_corner;
                *this->At(width - 2,0) = border_corner;
            }
            if(border[2]){
                for(int i = 1; i < height - 1; i++){
                    *this->At(0, i) = border_edge;
                    *this->At(1, i) = border_edge;
                }
                *this->At(0, 0) = border_corner;
                *this->At(1, 0) = border_corner;
                *this->At(0, height - 1) = border_corner;
                *this->At(1, height - 1) = border_corner;                 
            }
            if(border[3]){
                for(int i = 1; i < width - 1; i++){
                    *this->At(i, height - 1) = border_edge;
                }
                *this->At(0, height - 1) = border_corner;
                *this->At(1, height - 1) = border_corner;
                *this->At(width - 1, height - 1) = border_corner;
                *this->At(width - 2, height - 1) = border_corner;               
            }
        }
};
