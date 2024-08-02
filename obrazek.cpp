#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <atomic>
#include "obrazek.h"

const int WIDTH = 1000, HEIGHT = 1000;
const Uint32 SWITCH_INTERVAL = 1000; // Interval in milliseconds

std::atomic<bool> sequenceActivated(false); // Define the external variable

int picture_and_music(int argc, char* argv[])
{
    SDL_Surface* imageSurface = NULL;
    SDL_Surface* windowSurface = NULL;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("BOMBA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    windowSurface = SDL_GetWindowSurface(window);

    // Check that the window was successfully created
    if (NULL == window)
    {
        // In the case that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cout << "Could not initialize SDL_image: " << IMG_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    Mix_Music* bgMusic = Mix_LoadMUS("bomb.mp3");
    if (bgMusic == NULL) {
        std::cout << "Failed to load beat music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    Mix_Chunk* keySound = Mix_LoadWAV("button.wav");
    if (keySound == NULL) {
        std::cout << "Failed to load key press sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return 1;
    }

    SDL_Event windowEvent;

    imageSurface = IMG_Load("BOMBA.png");

    if (NULL == imageSurface)
    {
        std::cout << "SDL could not load image! SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    bool running = true;
    bool imageVisible = true;
    Uint32 imageStartTime = SDL_GetTicks(); // Get the current time

    // Sequence tracking
    std::vector<SDL_Keycode> keySequence = { SDLK_7, SDLK_3, SDLK_5, SDLK_5, SDLK_6, SDLK_0, SDLK_8 };
    std::vector<SDL_Keycode> userSequence;
    sequenceActivated = false; // Initialize the external flag
    Uint32 switchStartTime = 0;
    int currentImage = 0;
    SDL_Surface* altImages[2] = { IMG_Load("TIK.png"), IMG_Load("TAK2.png") }; // Load alternate images

    while (running)
    {
        while (SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT)
            {
                running = false;
                break;
            }
            else if (windowEvent.type == SDL_KEYDOWN)
            {
                // Play the button.wav sound effect
                Mix_PlayChannel(-1, keySound, 0);

                if (sequenceActivated)
                {
                    // Do nothing if the sequence is already activated
                    continue;
                }

                userSequence.push_back(windowEvent.key.keysym.sym);

                if (userSequence.size() > keySequence.size())
                {
                    userSequence.erase(userSequence.begin()); // Keep the last n keys where n is the length of keySequence
                }

                if (userSequence == keySequence)
                {
                    sequenceActivated = true;
                    switchStartTime = SDL_GetTicks();
                    currentImage = 0; // Start with the first alternate image

                    // Start playing music
                    Mix_PlayMusic(bgMusic, -1); // Play the music indefinitely
                }
                else
                {
                    SDL_FreeSurface(imageSurface);

                    switch (windowEvent.key.keysym.sym)
                    {
                    case SDLK_c:
                        imageSurface = IMG_Load("BOMBAC.png");
                        break;
                    case SDLK_h:
                        imageSurface = IMG_Load("BOMBA#.png");
                        break;
                    case SDLK_0:
                        imageSurface = IMG_Load("BOMBA0.png");
                        break;
                    case SDLK_1:
                        imageSurface = IMG_Load("BOMBA1.png");
                        break;
                    case SDLK_2:
                        imageSurface = IMG_Load("BOMBA2.png");
                        break;
                    case SDLK_3:
                        imageSurface = IMG_Load("BOMBA3.png");
                        break;
                    case SDLK_4:
                        imageSurface = IMG_Load("BOMBA4.png");
                        break;
                    case SDLK_5:
                        imageSurface = IMG_Load("BOMBA5.png");
                        break;
                    case SDLK_6:
                        imageSurface = IMG_Load("BOMBA6.png");
                        break;
                    case SDLK_7:
                        imageSurface = IMG_Load("BOMBA7.png");
                        break;
                    case SDLK_8:
                        imageSurface = IMG_Load("BOMBA8.png");
                        break;
                    case SDLK_9:
                        imageSurface = IMG_Load("BOMBA9.png");
                        break;
                    default:
                        imageSurface = IMG_Load("BOMBA.png"); // Load default image if no valid key is pressed
                        break;
                    }

                    if (NULL == imageSurface)
                    {
                        std::cout << "SDL could not load image! SDL Error: " << SDL_GetError() << std::endl;
                        return 1;
                    }

                    // Reset timer when a new image is loaded
                    imageStartTime = SDL_GetTicks();
                    imageVisible = true;
                }
            }
        }

        if (sequenceActivated)
        {
            if (SDL_GetTicks() - switchStartTime > SWITCH_INTERVAL)
            {
                currentImage = 1 - currentImage; // Switch between 0 and 1
                switchStartTime = SDL_GetTicks();
            }

            SDL_BlitSurface(altImages[currentImage], NULL, windowSurface, NULL);
        }
        else
        {
            // Check if the image has been displayed for more than 0.5 seconds
            if (SDL_GetTicks() - imageStartTime > 500)
            {
                // Load the BOMBA.png image again
                SDL_FreeSurface(imageSurface);
                imageSurface = IMG_Load("BOMBA.png");
                if (NULL == imageSurface)
                {
                    std::cout << "SDL could not load image! SDL Error: " << SDL_GetError() << std::endl;
                    return 1;
                }

                // Reset timer to prevent continuous loading
                imageStartTime = SDL_GetTicks();
            }

            SDL_BlitSurface(imageSurface, NULL, windowSurface, NULL);
        }

        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(imageSurface);
    SDL_FreeSurface(windowSurface);
    SDL_FreeSurface(altImages[0]);
    SDL_FreeSurface(altImages[1]);

    imageSurface = NULL;
    windowSurface = NULL;
    altImages[0] = NULL;
    altImages[1] = NULL;

    Mix_FreeChunk(keySound);
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
