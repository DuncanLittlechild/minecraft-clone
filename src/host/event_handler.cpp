#include "event_handler.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "game_api.h"

SDL_AppResult HandleEvent(SDL_Event* event, UserInput& input)
{
    switch(event->type) {
        case (SDL_EVENT_QUIT) : {
            return SDL_APP_SUCCESS;
        } break;
                case(SDL_EVENT_KEY_DOWN) :{
            switch(event->key.scancode){
                case (SDL_SCANCODE_W):{
                    input.keys |= UserInput::w_key;
                } break;
                case (SDL_SCANCODE_S):{
                    input.keys |= UserInput::s_key;
                } break;
                case (SDL_SCANCODE_A):{
                    input.keys |= UserInput::a_key;
                }break;
                case (SDL_SCANCODE_D):{
                    input.keys |= UserInput::d_key;
                }break;
                case (SDL_SCANCODE_UP):{
                    input.keys |= UserInput::up_key;
                }break;
                case (SDL_SCANCODE_DOWN):{
                    input.keys |= UserInput::down_key;
                }break;
                case (SDL_SCANCODE_LEFT):{
                    input.keys |= UserInput::left_key;
                }break;
                case (SDL_SCANCODE_RIGHT):{
                    input.keys |= UserInput::right_key;
                }break;
            }
        } break;
        case(SDL_EVENT_KEY_UP) :{
            switch(event->key.scancode){             
                case (SDL_SCANCODE_W):{
                    input.keys &= ~UserInput::w_key;
                }break;
                case (SDL_SCANCODE_S):{
                    input.keys &= ~UserInput::s_key;
                }break;
                case (SDL_SCANCODE_A):{
                    input.keys &= ~UserInput::a_key;
                }break;
                case (SDL_SCANCODE_D):{
                    input.keys &= ~UserInput::d_key;
                }break;
                case (SDL_SCANCODE_UP):{
                    input.keys &= ~UserInput::up_key;
                }break;
                case (SDL_SCANCODE_DOWN):{
                    input.keys &= ~UserInput::down_key;
                }break;
                case (SDL_SCANCODE_LEFT):{
                    input.keys &= ~UserInput::left_key;
                }break;
                case (SDL_SCANCODE_RIGHT):{
                    input.keys &= ~UserInput::right_key;
                }break;
            }
        } break;
        case (SDL_EVENT_MOUSE_BUTTON_DOWN):{
            switch(event->button.button){
                case(SDL_BUTTON_LEFT):{
                    float x, y;
                    SDL_GetMouseState(&x, &y);
                    input.but |= UserInput::LCLICK;
                    appData->input.mouseX = x;
                    appData->input.mouseY = y;
                } break;
                case(SDL_BUTTON_RIGHT):{
                    appData->input.clickFlags |= UserInput::RCLICK;
                } break;
            }
        } break;
        case(SDL_EVENT_MOUSE_MOTION):{
            
        }
    }

    return SDL_APP_CONTINUE;
}