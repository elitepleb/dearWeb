
// DearWeb, imgui + sdl + lua running on the web with emscripten

// ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_internal.h"

// SDL stuff
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

// Emscripten stuff
#include <emscripten.h>
#include <emscripten/html5.h>

#include "site/site.h"

SDL_Window *g_Window = NULL;
SDL_GLContext g_GLContext = NULL;

void main_loop( void * );

int main( int, char ** ) {

  //SDL initialization

  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) !=
      0 ) {
    printf( "Error: %s\n", SDL_GetError() );
    return -1;
  }

  //OpenGl settings
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode( 0, &current );
  SDL_WindowFlags window_flags = SDL_WindowFlags(
                                   SDL_WINDOW_OPENGL |
                                   SDL_WINDOW_RESIZABLE |
                                   SDL_WINDOW_ALLOW_HIGHDPI
                                 );

  int width, height;
  emscripten_get_canvas_element_size( "#canvas", &width, &height );

  g_Window = SDL_CreateWindow( "Dear ImGui Emscripten Lua demo",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width, height, window_flags
                             );

  g_GLContext = SDL_GL_CreateContext( g_Window );

  if( !g_GLContext ) {
    fprintf( stderr, "Failed to initialize WebGL context!\n" );
    return 1;
  }

  //SDL_GL_SetSwapInterval( 1 ); // Enable vsync

  //Dear ImGui setup
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableKeyboard;

  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableGamepad;

  // No files, means no settings
  io.IniFilename = NULL;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup bindings
  ImGui_ImplSDL2_InitForOpenGL( g_Window, g_GLContext );
  ImGui_ImplOpenGL3_Init( "#version 100" );

  Site::Init();

  emscripten_set_main_loop_arg( main_loop, NULL, 0, true );
}

void main_loop( void * ) {
  // input polling
  SDL_Event event;

  while( SDL_PollEvent( &event ) ) {
    ImGui_ImplSDL2_ProcessEvent( &event );
  }

  // Make a frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame( g_Window );
  ImGui::NewFrame();

  // Site logic
  Site::Tick();

  // Render a frame
  ImGui::Render();
  SDL_GL_MakeCurrent( g_Window, g_GLContext );
  ImGuiIO &io = ImGui::GetIO();
  glViewport( 0, 0, ( int )io.DisplaySize.x, ( int )io.DisplaySize.y );

  static ImVec4 clear_color = ImVec4( 0.66f, 0.66f, 0.66f, 1.00f );
  glClearColor( clear_color.x, clear_color.y, clear_color.z, clear_color.w );
  glClear( GL_COLOR_BUFFER_BIT );

  ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
  SDL_GL_SwapWindow( g_Window );
}
