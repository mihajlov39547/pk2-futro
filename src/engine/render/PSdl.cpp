//#########################
//Pekka Kana 2
//Copyright (c) 2003 Janne Kivilahti
//#########################
#include "engine/PRender.hpp"

#include "engine/PLog.hpp"

#include "engine/render/PSdl.hpp"

#include <SDL.h>

void PSdl::load_ui_texture(void* surface) {

    ui_surface = (SDL_Surface*)surface;
    ui_texture = SDL_CreateTextureFromSurface(renderer, (SDL_Surface*)ui_surface);
    if (ui_texture == NULL) {

        PLog::Write(PLog::ERR, "PSdl", "Couldn't load texture!");

    }

}
void PSdl::render_ui(PRender::FRECT src, PRender::FRECT dst, float alpha) {

    RenderOptions opt;

    opt.src = src;
    opt.dst = dst;
    opt.alpha = alpha;

    render_list.push_back(opt);

}

void PSdl::clear_screen() {

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

}

void PSdl::set_screen(PRender::FRECT screen_dst) {

    screen_dest = screen_dst;

}

void PSdl::set_screen_source(PRender::RECT screen_src) {

    this->screen_src = screen_src;

}

int PSdl::set_shader(int mode) {

    if (mode == PRender::SHADER_NEAREST) {
        if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0") == SDL_TRUE)
            return 0;
    } else if (mode == PRender::SHADER_LINEAR) {
        if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2") == SDL_TRUE)
            return 0;
    }
    
    return 1;
}

int PSdl::set_vsync(bool set) {

    if (renderer) {

        ui_texture = NULL;
        SDL_DestroyRenderer(renderer);

    }

    Uint32 sync = set? SDL_RENDERER_PRESENTVSYNC : 0;

    renderer = SDL_CreateRenderer(curr_window, -1, SDL_RENDERER_ACCELERATED | sync);
    if (!renderer) {

        PLog::Write(PLog::FATAL, "PSdl", "Couldn't create renderer!");
        return -1;

    }

    SDL_RenderClear(renderer);

    return 0;

}

void PSdl::update(void* _buffer8) {

    SDL_Surface* buffer8 = (SDL_Surface*)_buffer8;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, buffer8);

    SDL_RenderClear(renderer);

    int w, h;
    if (SDL_GetRendererOutputSize(renderer, &w, &h) != 0)
        PRender::get_window_size(&w, &h);

    SDL_Rect dst;
    dst.x = screen_dest.x * w;
    dst.y = screen_dest.y * h;
    dst.w = screen_dest.w * w;
    dst.h = screen_dest.h * h;

    SDL_Rect src = {screen_src.x, screen_src.y, screen_src.w, screen_src.h};
    if (src.w <= 0 || src.h <= 0) {
        src.x = 0;
        src.y = 0;
        src.w = buffer8->w;
        src.h = buffer8->h;
    }

    SDL_RenderCopy(renderer, texture, &src, &dst);

    float prop_x = (float)w;
    float prop_y = (float)h;
    
    if (ui_surface) {

        if (!ui_texture)
            ui_texture = SDL_CreateTextureFromSurface(renderer, (SDL_Surface*)ui_surface);

        for (auto opt : render_list) {
            
            u8 mod = opt.alpha * 256;
            if (mod == 0)
                continue;
            
            SDL_SetTextureAlphaMod(ui_texture, mod);
            
            SDL_Rect dst;
            dst.x = opt.dst.x * prop_x;
            dst.y = opt.dst.y * prop_y;
            dst.w = opt.dst.w * prop_x;
            dst.h = opt.dst.h * prop_y;

            SDL_Rect src;
            src.x = opt.src.x * 1024;
            src.y = opt.src.y * 1024;
            src.w = opt.src.w * 1024;
            src.h = opt.src.h * 1024;

            SDL_RenderCopy(renderer, ui_texture, &src, &dst);
            
        }

        render_list.clear();

    }

    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);

}

PSdl::PSdl(int width, int height, void* window) {

    curr_window = (SDL_Window*)window;

    renderer = SDL_CreateRenderer(curr_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {

        PLog::Write(PLog::FATAL, "PSdl", "Couldn't create renderer!");

    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    SDL_RenderClear(renderer);

}

PSdl::~PSdl() {

    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(ui_surface);
    PLog::Write(PLog::DEBUG, "PSdl", "Terminated");

}
