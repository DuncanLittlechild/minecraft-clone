#include "event_handler.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "game_api.h"

// Key presses rely on values persisting from loop to loop to detect
// when a key is held, while the mouse input only cares about
// what just happened that loop
void ResetInput(UserInput& input)
{
    input.mouse = 0;
    input.mYRel = 0.0f;
    input.mXRel = 0.0f;
}

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
                // For now, q quits the game
                case(SDL_SCANCODE_Q):{
                    return SDL_APP_SUCCESS;
                };
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
                    input.mouse |= UserInput::LEFT_CLICK;
                } break;
                case(SDL_BUTTON_RIGHT):{
                    input.mouse |= UserInput::RIGHT_CLICK;
                } break;
            }
        } break;
        case(SDL_EVENT_MOUSE_MOTION):{
            input.mouse |= UserInput::MOUSE_MOVED;
            input.mXAbs = event->motion.x;
            input.mYAbs = event->motion.y;
            input.mXRel = event->motion.xrel;
            input.mYRel = event->motion.yrel;
        }
    }

    return SDL_APP_CONTINUE;
}