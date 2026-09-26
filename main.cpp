#include "bits/stdc++.h"

#include <conio.h>

#include ".h\elements.h"

using namespace std;

int main(){
    Window scrn(0,0,120,30);
    scrn.SetBg(0x80);

    Window* window1 = scrn.CreateSon<Window>(2,2,20,26,"App1");
    window1->SetBg(0x07);

    Label* label0 = window1->CreateSon<Label>(0,1,20,5);
    label0->content = "These are words with left-alignment.";
    label0->v_aligment = 1;
    
    Label* label1 = window1->CreateSon<Label>(0,6,20,5);
    label1->content = "These are words with middle-alignment.";
    label1->aligment = 1;
    label1->v_aligment = 1;
    
    Label* label2 = window1->CreateSon<Label>(0,11,20,5);
    label2->content = "These are words with right-alignment.";
    label2->aligment = 2;
    label2->v_aligment = 1;

    Window* bottom = scrn.CreateSon<Window>(0,29,120,1);
    bottom->SetBg(0x70);
    
    Button* btn0 = bottom->CreateSon<Button>(0,0,10,1);
    btn0->label->content = "App1";
    btn0->off_click = [window1](){
        window1->render = !window1->render;
    };

    Picture* pic0 = window1->CreateSon<Picture>(0,16,20,10);
    *pic0->At(2,1) = CHAR_INFO{'#', 0x12};
    *pic0->At(2,3) = CHAR_INFO{'#', 0x34};
    *pic0->At(2,5) = CHAR_INFO{'#', 0x56};
    *pic0->At(2,7) = CHAR_INFO{'#', 0x78};

    //return 0;

    InitConsole(L"test");
    scrn.FlushAll();

    Element* hit_ele = nullptr;
    DWORD button_state = 0;
    SHORT x = 0;
    SHORT y = 0;

    HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD ir;
    DWORD read;
    while(1){
        //get input
        if (!ReadConsoleInput(h_in, &ir, 1, &read) || read != 1) continue;
        if (ir.EventType != MOUSE_EVENT) continue;
        MOUSE_EVENT_RECORD& m = ir.Event.MouseEvent;

        //invoke move-event
        x = m.dwMousePosition.X;
        y = m.dwMousePosition.Y;
        Element* curr_hit = scrn.hittest(x, y);
        if(hit_ele != nullptr) hit_ele->Leave();
        if(!(button_state & FROM_LEFT_1ST_BUTTON_PRESSED)) hit_ele = curr_hit;
        hit_ele->OnHover();

        //invoke mouse-event
        DWORD curr_btn = m.dwButtonState;
        printf("%d\r", curr_btn);  
        if(button_state & FROM_LEFT_1ST_BUTTON_PRESSED){
            if(typeid(*hit_ele) == typeid(Button)){
                static_cast<Button*>(hit_ele)->Hold(Mouse(x, y, button_state));
            } 
        }
        if((curr_btn & FROM_LEFT_1ST_BUTTON_PRESSED) &&
            !(button_state & FROM_LEFT_1ST_BUTTON_PRESSED)){
                button_state = curr_btn;
                if(hit_ele != nullptr && typeid(*hit_ele) == typeid(Button)){
                    static_cast<Button*>(hit_ele)->OnClick(Mouse(x, y, button_state));
                }   
        }
        if(!(curr_btn & FROM_LEFT_1ST_BUTTON_PRESSED) &&
            (button_state & FROM_LEFT_1ST_BUTTON_PRESSED)){
                button_state = curr_btn;
                if(hit_ele != nullptr && typeid(*hit_ele) == typeid(Button)){
                    static_cast<Button*>(hit_ele)->OffClick();
                }   
        }

        //fresh
        scrn.FlushAll();
        Sleep(20);
    }

    system("cls");
    return 0;
}