#pragma once
#include "ui.h"

class Picture: public Element{
    public:

        Picture(SHORT _col, SHORT _row, SHORT _width, SHORT _height, std::string _title = ""):
            Element(_col, _row, _width, _height)
        {}

        void Render(Element* target){
            if(target != nullptr){
                for(int i = 0; i < width; i++){
                    for(int j = 0; j < height; j++){
                        if(0 <= col + i && col + i < target->width && 0 <= row + j && row + j < target->height)
                        *target->At(col + i, row + j) = *this->At(i, j);
                    }
                }
            }
        }

    private:
};
