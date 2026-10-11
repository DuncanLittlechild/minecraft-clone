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
                    input.keys |= UserInput::W_PRESSED;
                } break;
                case (SDL_SCANCODE_S):{
                    input.keys |= UserInput::S_PRESSED;
                } break;
                case (SDL_SCANCODE_A):{
                    input.keys |= UserInput::A_PRESSED;
                }break;
                case (SDL_SCANCODE_D):{
                    input.keys |= UserInput::D_PRESSED;
                }break;
                case (SDL_SCANCODE_UP):{
                    input.keys |= UserInput::UP_PRESSED;
                }break;
                case (SDL_SCANCODE_DOWN):{
                    input.keys |= UserInput::DOWN_PRESSED;
                }break;
                case (SDL_SCANCODE_LEFT):{
                    input.keys |= UserInput::LEFT_PRESSED;
                }break;
                case (SDL_SCANCODE_RIGHT):{
                    input.keys |= UserInput::RIGHT_PRESSED;
                }break;
            }
        } break;
        case(SDL_EVENT_KEY_UP) :{
            switch(event->key.scancode){             
                case (SDL_SCANCODE_W):{
                    input.keys &= ~UserInput::W_PRESSED;
                }break;
                case (SDL_SCANCODE_S):{
                    input.keys &= ~UserInput::S_PRESSED;
                }break;
                case (SDL_SCANCODE_A):{
                    input.keys &= ~UserInput::A_PRESSED;
                }break;
                case (SDL_SCANCODE_D):{
                    input.keys &= ~UserInput::D_PRESSED;
                }break;
                case (SDL_SCANCODE_UP):{
                    input.keys &= ~UserInput::UP_PRESSED;
                }break;
                case (SDL_SCANCODE_DOWN):{
                    input.keys &= ~UserInput::DOWN_PRESSED;
                }break;
                case (SDL_SCANCODE_LEFT):{
                    input.keys &= ~UserInput::LEFT_PRESSED;
                }break;
                case (SDL_SCANCODE_RIGHT):{
                    input.keys &= ~UserInput::RIGHT_PRESSED;
                }break;
            }
        } break;
        case (SDL_EVENT_MOUSE_BUTTON_DOWN):{
            switch(event->button.button){
                case(SDL_BUTTON_LEFT):{
                    input.but |= UserInput::LEFT_CLICK;
                } break;
                case(SDL_BUTTON_RIGHT):{
                    input.but |= UserInput::RIGHT_CLICK;
                } break;
            }
        } break;
        case(SDL_EVENT_MOUSE_MOTION):{
            input.mXAbs = event->motion.x;
            input.mYAbs = event->motion.y;
            input.mXRel = event->motion.xrel;
            input.mYRel = event->motion.yrel;
        }
    }

    return SDL_APP_CONTINUE;
}