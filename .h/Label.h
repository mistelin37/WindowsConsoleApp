#pragma once
#include "ui.h"

class Label: public Element{
    public:
        std::string content;
        int aligment = 0;//left, middle, right
        int v_aligment = 0;//up, middle, down
        bool transparent = true;

        Label(SHORT _col, SHORT _row, SHORT _width, SHORT _height):
            Element(_col, _row, _width, _height)
        {}

        void Render(Element* target) override {

            std::string word = "";
            std::vector<std::string> words;
            for(int i = 0; i < content.size(); i++){
                if(content[i] == ' '){
                    words.push_back(word);
                    word = "";
                }
                else{
                    word += content[i];
                }
            }
            if(!word.empty()){
                words.push_back(word);
            }

            int x = 0;
            int y = 0;
            std::string line = "";
            std::vector<std::string> lines;
            for(std::string i : words){
                if(x + i.size() >= width){
                    line.pop_back();
                    lines.push_back(line);
                    y++;
                    line = "";
                    x = 0;
                }
                if(y >= height || i.size() > width){
                    break;
                }
                line += i + " ";
                x += i.size() + 1;
            }
            if(!line.empty()){
                line.pop_back();
                lines.push_back(line);
            }

            if(transparent) format = target->format;
            y = 0;

            if(v_aligment == 0) y += 0;
            if(v_aligment == 1) y += (height - lines.size()) / 2;
            if(v_aligment == 2) y += height - lines.size();
            for(std::string i : lines){
                x = 0;

                int space;
                if(aligment == 0) space = 0;
                if(aligment == 1) space = (width - i.size()) / 2;
                if(aligment == 2) space = width - i.size();
                while(space--){
                    if(0 <= col + x && col + x < target->width && 0 <= row + y && row + y < target->height){
                        *target->At(col + x, row + y) = CHAR_INFO{L' ', format};
                    }
                    x++;
                }

                for(char ch : i){
                    if(0 <= col + x && col + x < target->width && 0 <= row + y && row + y < target->height){
                        *target->At(col + x, row + y) = CHAR_INFO{(wchar_t)ch, format};
                    }
                    x++;
                }
                y++;
            }
        }
};
