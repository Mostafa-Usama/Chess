#include <SDL.h>
#include <SDL_image.h>
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

    else if (abs(piece) == 2){
        int dx = abs((dest->x / size) - (x / size));
        int dy = abs((dest->y / size) - (y / size));
        if (dx == 1 && dy == 2 || dx == 2 && dy == 1) {
            return true;
        }
        else
            return false;
    } // knight 

    else // other pieces
        return true;
}

int main(int argc, char *argv[]){
   
    SDL_Window *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 455, 455, SDL_WINDOW_SHOWN);
    SDL_Renderer *render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  

    SDL_Event event;

    SDL_Texture *board = IMG_LoadTexture(render, "res/images/board0.png");
    SDL_Texture *piceses = IMG_LoadTexture(render, "res/images/figures.png");

    vector <SDL_Rect *>allPiecesDest;
    vector <SDL_Rect *>allPiecesSrc;
    vector<int> colors;

    loadPieces(render, piceses, allPiecesDest, allPiecesSrc, colors);
    
    bool gameRunning = true;
    bool isMoving = false;
    int turn = 1;
    int dx, dy, index;
    int oldX, oldY;
    SDL_Rect *currentPiece = NULL;

    while (gameRunning){
        int mouseX, mouseY;
        Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

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
                           cout << "White turn" << endl;
                           isMoving = true;
                           index = i;
                           currentPiece = allPiecesDest[i];
                           dx = mouseX - currentPiece->x;
                           dy = mouseY - currentPiece->y;
                           oldX = currentPiece->x;
                           oldY = currentPiece->y;
                       }
                       
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP && currentPiece != NULL)
            {
               
                isMoving = false;
                turn = -turn;
                

                int x = (currentPiece->x + size / 2) / size;
                int y = (currentPiece->y + size / 2) / size;

                currentPiece->x = size * x;
                currentPiece->y = size * y;

                if(isValidMove(colors[index], currentPiece, oldX, oldY, allPiecesDest, index)){
                    // sounds
                    for (int i = allPiecesDest.size() - 1; i >= 0; i--)
                    {
                    if (i == index)
                        continue;
                    if (SDL_HasIntersection(currentPiece, allPiecesDest[i]))
                        {   
                        if ( (turn == -1 && colors[i] < 0) || (turn == 1 && colors[i] > 0) ){

                        allPiecesDest.erase(allPiecesDest.begin() + i);
                        allPiecesSrc.erase(allPiecesSrc.begin() + i);
                        colors.erase(colors.begin() + i);
                        
                        }
                        else {
                        
                        turn = -turn;
                        currentPiece->x = oldX;
                        currentPiece->y = oldY;
                        }
                    }
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
        }
        SDL_RenderClear(render);
        renderBoard(render, board);
        renderPieces(render, piceses, allPiecesDest, allPiecesSrc);
        SDL_RenderPresent(render);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

