#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
using namespace std;

int size = 56;

void renderBoard(SDL_Renderer* render, SDL_Texture* texture){
    SDL_Rect dst;
    dst.x = 0;
    dst.y = 0;
    dst.w = 455;
    dst.h = 455;
    SDL_RenderCopy(render, texture, NULL, &dst);
}

void loadPieces(SDL_Renderer *render, SDL_Texture *text, vector<SDL_Rect *> &allD, vector<SDL_Rect *> &allS, vector<int> &colors)
{
    
    int index = 0;
    int boardPieces[8][8] = {
        {-1, -2, -3, -4, -5, -3, -2, -1},
        {-6, -6, -6, -6, -6, -6, -6, -6},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {6, 6, 6, 6, 6, 6, 6, 6},
        {1, 2, 3, 4, 5, 3, 2, 1}
    };

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            int n = boardPieces[i][j];
            if (n == 0) continue;
            int pos = abs(n) - 1;
            int side = n > 0 ? 1 : 0;
            colors.push_back(n);
            SDL_Rect *src = new SDL_Rect;
            src->x = size * pos;
            src->y = size * side;
            src->h = size;
            src->w = size;

            SDL_Rect *dst = new SDL_Rect;
            dst->x = j * size;
            dst->y = i * size;
            dst->w = size;
            dst->h = size;
            SDL_RenderCopy(render, text, src, dst);
            allD.push_back(dst);
            allS.push_back(src);
            
            index++;
            
        }
    }
}

void renderPieces(SDL_Renderer *render, SDL_Texture *text, vector <SDL_Rect *> &allD, vector <SDL_Rect *> &allS)
{

    for (int i = 0; i < allD.size(); i++)
    {

        SDL_RenderCopy(render, text, allS[i], allD[i]);
    }
}

bool isMouseInRect(int mouseX, int mouseY, SDL_Rect *rect)
{
    return mouseX >= rect->x && mouseX <= rect->x + rect->w &&
           mouseY >= rect->y && mouseY <= rect->y + rect->h;
}

bool isValidMove(int piece, SDL_Rect *dest, int x, int y, vector<SDL_Rect *> &allD, int index)
{
    if (dest->x == x && dest->y == y)
    { // same square move
        return false;
    }
    if(abs(piece) == 1){ // Rock
        if ( (dest->x == x && dest->y != y) || (dest->x != x && dest->y == y) ){
            
            // cout << "Rock Valid Move" << endl;
            if (x / size == dest->x / size){
                int minY = min(y, dest->y) / size;
                int maxY = max(y, dest->y) / size;
                for (int i = minY; i <= maxY; i++){
                    SDL_Rect tmpPos = {x, i * size, size, size};
                    for (int j = allD.size() - 1; j >= 0; j--){
                        if (j == index)
                            continue;
                        if (SDL_HasIntersection(&tmpPos, allD[j]))
                        {
                            if (dest->y != tmpPos.y){
                                cout << "blocked" << endl;
                                return false; 
                                } 
                        }
                      
                    }
                }
                
            }
            if (y / size == dest->y / size)
            {
                int minX = min(x, dest->x) / size;
                int maxX = max(x, dest->x) / size;
                for (int i = minX; i < maxX; i++)
                {
                    SDL_Rect tmpPos = {i * size, y, size, size};
                    for (int j = allD.size() - 1; j >= 0; j--)
                    {
                        if (j == index)
                                continue;
                        if (SDL_HasIntersection(&tmpPos, allD[j]))
                        {
                                if (dest->x != tmpPos.x)
                                {
                                cout << "blocked" << endl;
                                return false;
                                }
                        }
                    }
                }
            }
            return true;
        }
        else 
            return false;
    }

    else if (abs(piece) == 2) // knight
    { 
        int dx = abs((dest->x / size) - (x / size));
        int dy = abs((dest->y / size) - (y / size));
        if (dx == 1 && dy == 2 || dx == 2 && dy == 1) {
            return true;
        }
        else
            return false;
    }

    else if (abs(piece) == 3) // Bishop
    { 
        int dx = abs((dest->x / size) - (x / size));
        int dy = abs((dest->y / size) - (y / size));
        if (dx == dy)
        {
            // int minY = min(y, dest->y) / size;
            // int maxY = max(y, dest->y) / size;
            // int minX = min(x, dest->x) / size;
            // int maxX = max(x, dest->x) / size;
            int xDir = (dest->x > x) ? 1 : -1;
            int yDir = (dest->y > y) ? 1 : -1;
            //cout << minX << " " << minY << " " << maxX <<"  "<< maxY << endl;
            for (int i = 1; i <= dx; i++)
            {
                int checkX = x + i * xDir * size;
                int checkY = y + i * yDir * size;
                SDL_Rect tmpPos = {checkX, checkY, size, size};
                for (int j = allD.size() - 1; j >= 0; j--)
                {
                    if (j == index)
                        continue;
                    if (SDL_HasIntersection(&tmpPos, allD[j]))
                    {
                        if (dest->x != tmpPos.x || dest->y != tmpPos.y)
                        {
                                cout << "blocked" << endl;
                                return false;
                        }
                    }
                }
            }
            return true;
        }
        else
        return false;
    }
    else // other pieces
        return true;
}

int main(int argc, char *argv[]){
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 455, 455, SDL_WINDOW_SHOWN);
    SDL_Renderer *render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

    SDL_Event event;

    SDL_Texture *board = IMG_LoadTexture(render, "res/images/board1.png");
    SDL_Texture *piceses = IMG_LoadTexture(render, "res/images/figures.png");

    vector <SDL_Rect *>allPiecesDest;
    vector <SDL_Rect *>allPiecesSrc;
    vector<int> colors;

    Mix_Chunk *move = Mix_LoadWAV("res/Sounds/move-self.wav");
    Mix_Chunk *capture = Mix_LoadWAV("res/Sounds/capture.wav");

    loadPieces(render, piceses, allPiecesDest, allPiecesSrc, colors);
    
    bool gameRunning = true;
    bool isMoving = false;
    int turn = 1;
    int dx, dy, index;
    int oldX, oldY;
    SDL_Rect *currentPiece = NULL;

    SDL_Rect oldPos;
    SDL_Rect newPos;
    SDL_Rect movePos;
    bool captured = false;
    bool moved = true;
    while (gameRunning){
        int mouseX, mouseY;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
        
        SDL_RenderClear(render);
        renderBoard(render, board);
        
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT){
                gameRunning = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN){
                for (int i = 0; i < allPiecesDest.size(); i++){
                   if (isMouseInRect(mouseX, mouseY, allPiecesDest[i])){
                       if (turn == 1 && colors[i] > 0 || turn == -1 && colors[i] < 0)
                       {
                                isMoving = true;
                                index = i;
                                currentPiece = allPiecesDest[i];
                                dx = mouseX - currentPiece->x;
                                dy = mouseY - currentPiece->y;
                                oldX = currentPiece->x;
                                oldY = currentPiece->y;
                                movePos.x = oldX + 2;
                                movePos.y = oldY + 2;
                                movePos.h = size;
                                movePos.w = size;
                       }
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP && currentPiece != NULL)
            {
                // SDL_SetRenderDrawColor(render, 255, 0, 255, 100);
                // SDL_RenderFillRect(render, currentPiece);
                isMoving = false;
                captured = false;
                
                turn = -turn;
                

                int x = (currentPiece->x + size / 2) / size;
                int y = (currentPiece->y + size / 2) / size;

                currentPiece->x = size * x;
                currentPiece->y = size * y;

                if(isValidMove(colors[index], currentPiece, oldX, oldY, allPiecesDest, index)){
                    // sounds
                    moved = true;
                    for (int i = allPiecesDest.size() - 1; i >= 0; i--)
                    {
                       if (i == index)
                           continue;
                       if (SDL_HasIntersection(currentPiece, allPiecesDest[i]))
                       {
                           if ((turn == -1 && colors[i] < 0) || (turn == 1 && colors[i] > 0))
                            {
                                captured = true;
                                allPiecesDest.erase(allPiecesDest.begin() + i);
                                allPiecesSrc.erase(allPiecesSrc.begin() + i);
                                colors.erase(colors.begin() + i);
                                moved = true;
                            }
                            else
                            {

                                turn = -turn;
                                currentPiece->x = oldX;
                                currentPiece->y = oldY;
                                // newPos = NULL;
                                moved = false;
                            }
                        }
                        
                    }
                    if (moved){
                    captured ? Mix_PlayChannel(-1, capture, 0) : Mix_PlayChannel(-1, move, 0);

                    newPos.x = currentPiece->x + 2;
                    newPos.y = currentPiece->y + 2;
                    newPos.w = currentPiece->w;
                    newPos.h = currentPiece->h;
                
                    oldPos.x = oldX + 2;
                    oldPos.y = oldY + 2;
                    oldPos.h = size;
                    oldPos.w = size;
                    }
            }
            else
            {

                turn = -turn;
                currentPiece->x = oldX;
                currentPiece->y = oldY;
            }
                currentPiece = NULL;
                
            }
        }
      
        if (isMoving){
            currentPiece->x = mouseX - dx;
            currentPiece->y = mouseY - dy;
            // draw rectangle around selected piece
            SDL_SetRenderDrawColor(render, 0, 10, 20, 120);
            SDL_RenderFillRect(render, &movePos);
            SDL_SetRenderDrawColor(render, 0, 130, 240,255);
            SDL_RenderDrawRect(render, currentPiece);
        }
            // draw rectangle at new and old piece position
        SDL_SetRenderDrawColor(render, 0, 10, 20, 120);
        SDL_RenderFillRect(render, &newPos);
        SDL_RenderFillRect(render, &oldPos);

        renderPieces(render, piceses, allPiecesDest, allPiecesSrc);
        SDL_RenderPresent(render);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

