#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
using namespace std;

int size = 56;
SDL_Rect *whiteKing;
SDL_Rect *blackKing;



bool checkHorizontalVertical(int y, int destY, int x, int destX, vector<SDL_Rect *> &allD, int index){
    if (x  == destX)
    {
        int minX = min(y, destY) / size;
        int maxX = max(y, destY) / size;
        for (int i = minX; i < maxX; i++)
        {
            SDL_Rect tmpPos = {x , i * size, size, size};
            for (int j = allD.size() - 1; j >= 0; j--)
            {
                if (j == index)
                    continue;
                if (SDL_HasIntersection(&tmpPos, allD[j]))
                {
                    if (destY != tmpPos.y)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }
   else if (y == destY)
    {
        int minX = min(x, destX) / size;
        int maxX = max(x, destX) / size;
        for (int i = minX; i < maxX; i++)
        {
            SDL_Rect tmpPos = {i * size, y, size, size};
            for (int j = allD.size() - 1; j >= 0; j--)
            {
                if (j == index)
                    continue;
                if (SDL_HasIntersection(&tmpPos, allD[j]))
                {
                    if (destX != tmpPos.x)
                    {
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
bool checkDiagonal(int y, int destY, int x, int destX, vector<SDL_Rect *> &allD, int index){
    int dx = abs((destX / size) - (x / size));
    int dy = abs((destY / size) - (y / size));
    if (dx == dy)
    {
        int xDir = (destX > x) ? 1 : -1;
        int yDir = (destY > y) ? 1 : -1;

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
                    if (destX != tmpPos.x || destY != tmpPos.y)
                    {
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


void renderBoard(SDL_Renderer *render, SDL_Texture *texture)
{
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
            if (n == 5)
                whiteKing = dst;
            if (n == -5)
                blackKing = dst;
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

bool isValidMove(int piece, SDL_Rect *dest, int x, int y, vector<SDL_Rect *> &allD, int index){
    if (dest->x == x && dest->y == y)
    { // same square move
        return false;
    }

    if(abs(piece) == 1){ // Rock
        return (checkHorizontalVertical(y, dest->y, x, dest->x, allD, index));
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
        return checkDiagonal(y, dest->y, x, dest->x, allD, index);
    }
    else if (abs(piece) == 6){ // Pawns
        int side;
        piece > 0 ? side = 1 : side = -1;
        int dx = abs((dest->x / size) - (x / size));
        int dy = ((dest->y / size) - (y / size));
        if ((abs(dy) == 2 && y / size == 6 && side == 1 && dx == 0) || (abs(dy) == 2 && y / size == 1 && side == -1 && dx == 0))
        {
        for (int j = allD.size() - 1; j >= 0; j--)
            {
                if (j == index)
                    continue;
                if (SDL_HasIntersection(dest, allD[j]))
                {
                    
                    return false;
                }
               
            }
            return true;
        }
        if (( dy == 1 && side == -1 || dy == -1 && side == 1) && (dx == 1 || dx == 0)){
                // SDL_Rect tmpPos = {x, y + size * -side, size, size};
                // SDL_Rect tmpPos2 = {x + size, y + size * side, size, size};
                // SDL_Rect tmpPos3 = {x - size, y + size * side, size, size};
                for (int j = allD.size() - 1; j >= 0; j--)
                {
                    if (j == index)
                        continue;
                    if (SDL_HasIntersection(dest, allD[j]) && dx == 1) 
                    {
                        return true;
                    }
                    if (SDL_HasIntersection(dest, allD[j]))
                    {
                        return false;
                    }
                    
                }
                if (dx == 1)
                {
                    return false;
                }
        return true;
        }
        else
        return false;
    }
    else if (abs(piece) == 4){ // Queen

        return (checkHorizontalVertical(y, dest->y, x, dest->x, allD, index) ||
                checkDiagonal(y, dest->y, x, dest->x, allD, index));
    }
    else // king
    {
        int dx = abs((dest->x / size) - (x / size));
        int dy = abs((dest->y / size) - (y / size));
        if (dx <= 1 && dy <= 1)
            return true;
        else
            return false;
    }
}

int isInCheck(vector<SDL_Rect *> &allD, vector<int> color, int index)
{
    for (int i = 0; i < allD.size(); i++)
    {
        if (i == index) continue;
        if (color[i] > 0)
        {
            if (isValidMove(color[i], blackKing, allD[i]->x, allD[i]->y, allD, i))
            {
                    cout << color[i] << endl;
                    return 1;
            }
        }
        else if (color[i] < 0)
        {
            if (isValidMove(color[i], whiteKing, allD[i]->x, allD[i]->y, allD, i))
            {
                    return -1;
            }
        }
    }
    return 0;
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
    Mix_Chunk *checky = Mix_LoadWAV("res/Sounds/move-check.wav");

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
    int check = 0;
    bool allowed = true;

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
                check = isInCheck(allPiecesDest, colors, -1);
               
                if ((check ==  1 && turn == 1) || (check == -1 && turn == -1)){
                    allowed = false;
                    for (int i = allPiecesDest.size() - 1; i >= 0; i--)
                    {
                       if (i == index)
                                continue;
                       if (SDL_HasIntersection(currentPiece, allPiecesDest[i]))
                       {
                                check = isInCheck(allPiecesDest, colors, i);
                                if (check == 0) // here
                                {
                                    allowed = true;
                                    break;
                                }

                        }
                    }
                }
                else
                {
                    allowed = true;
                }
                if (isValidMove(colors[index], currentPiece, oldX, oldY, allPiecesDest, index) && allowed)
                {
                    // sounds
                    moved = true;
                    for (int i = allPiecesDest.size() - 1; i >= 0; i--)
                    {
                        if (i == index)
                            continue;
                        if (SDL_HasIntersection(currentPiece, allPiecesDest[i]))
                        {
                            check = isInCheck(allPiecesDest, colors, -1);
                            if ((turn == -1 && colors[i] < 0) || (turn == 1 && colors[i] > 0) ) // here
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
                    if (moved)
                    {
                        check = isInCheck(allPiecesDest, colors, -1);
                        if (check != 0)
                            Mix_PlayChannel(-1, checky, 0);
                        else
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
       if (check != 0){
            SDL_SetRenderDrawColor(render, 200, 0, 0, 100);
            if (check == 1 )
            {
                SDL_Rect black = {blackKing->x + 2,
                                  blackKing->y + 2,
                                  size,
                                  size};

                SDL_RenderFillRect(render, &black);
        }
        if (check == -1 ){
            SDL_Rect white = {whiteKing->x + 2,
                              whiteKing->y + 2,
                              size,
                              size};
            SDL_RenderFillRect(render, &white);
        }
        }

        renderPieces(render, piceses, allPiecesDest, allPiecesSrc);
        SDL_RenderPresent(render);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

