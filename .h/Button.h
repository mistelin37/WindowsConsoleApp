#pragma once
#include "ui.h"
#include "Label.h"

class Button: public Element{
    public:
        int state = 0;//Normal, Hover, Hold, disable
        WORD state_format[4] = {0x70,0xF0,0x07,0x80};
        bool enabled = true;
        bool focused = false;
        std::unique_ptr<Label> label;
        
        std::function<void(Mouse event)> on_click;
        std::function<void(Mouse event)> hold;
        std::function<void()> off_click;

        Button(SHORT _col, SHORT _row, SHORT _width, SHORT _height):
            Element(_col, _row, _width, _height)
        {
            label = std::make_unique<Label>(0, 0, _width, _height);
            label->aligment = 1;
            label->v_aligment = 1;
        }

        void SetFormat(WORD _format_normal, 
                            WORD _format_hover,
                            WORD _format_pressed,
                            WORD _format_disabled){
            state_format[0] = _format_normal;
            state_format[1] = _format_hover;
            state_format[2] = _format_pressed;
            state_format[3] = _format_disabled;
        }
        
        void FlushBg(){
            for(int i = 0; i < width * height; i++){
                content[i].Attributes = format;
            }
        }
        
        void Render(Element* target){
            format = state_format[state];
            FlushBg();
            label->Render(this);

            if(target != nullptr){
                for(int i = 0; i < width; i++){
                    for(int j = 0; j < height; j++){
                        if(0 <= col + i && col + i < target->width && 0 <= row + j && row + j < target->height)
                        *target->At(col + i, row + j) = *this->At(i, j);
                    }
                }
            }
        }

        bool OnHover() {
            if (state == 3) return false;
            state = 1;
            return true;
        }
        bool OnClick(Mouse event) {
            if (state == 3) return false;
            if(on_click) on_click(event);
            state = 2;
            return true;
        }
        bool Hold(Mouse event) {
            if (state == 3) return false;
            if(hold) hold(event);
            return true;
        }
        bool OffClick() {
            if (state == 3) return false;
            if(off_click) off_click();
            state = 1;
            return true;
        }
        bool Leave() {
            if (state == 3) return false;
            state = 0;
            return true;
        }

};