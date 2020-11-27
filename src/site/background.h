#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../sol/sol.h"
#include <emscripten/html5.h>

namespace Background {

  inline ImDrawList *Draw;

  void Begin() {
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
    ImGui::PushStyleColor( ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f } );

    ImGui::Begin( "##Background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs );

    ImGuiIO &io = ImGui::GetIO();

    ImGui::SetWindowPos( ImVec2( 0, 0 ), ImGuiCond_Always );
    ImGui::SetWindowSize( ImVec2( io.DisplaySize.x, io.DisplaySize.y ), ImGuiCond_Always );

    ImGui::PopStyleColor();
    ImGui::PopStyleVar( 2 );

    Draw = ImGui::FindWindowByName( "##Background" )->DrawList;
  }

  void End() {
    ImGui::FindWindowByName( "##Background" )->DrawList->PushClipRectFullScreen();

    ImGui::End();
  }

  void LuaBindings( sol::table &draw ) {
    //  line                     (const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);
    draw.set_function( "line", []( int x, int y, int xx, int yy, uint32_t color, float thick ) {
      Draw->AddLine( ImVec2( x, y ), ImVec2( xx, yy ), __builtin_bswap32( color ), thick );
    } );
    //  rect                     (const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawCornerFlags rounding_corners = ImDrawCornerFlags_All, float thickness = 1.0f);
    draw.set_function( "rect", []( int x, int y, int w, int h, uint32_t color, float thick ) {
      Draw->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), __builtin_bswap32( color ), 0, 0, thick );
    } );
    //  rectfill                 (const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawCornerFlags rounding_corners = ImDrawCornerFlags_All);
    draw.set_function( "rectfill", []( int x, int y, int w, int h, uint32_t color, float thick ) {
      Draw->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), __builtin_bswap32( color ), 0, 0, thick );
    } );
    //  rectfilledmulticolor     (const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    draw.set_function( "rectfillgradient", []( int x, int y, int w, int h, uint32_t color1, uint32_t color2, uint32_t color3, uint32_t color4 ) {
      Draw->AddRectFilledMultiColor( ImVec2( x, y ), ImVec2( x + w, y + h ), __builtin_bswap32( color1 ), __builtin_bswap32( color2 ), __builtin_bswap32( color3 ), __builtin_bswap32( color4 ) );
    } );
    //  quad                     (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness = 1.0f);
    draw.set_function( "quad", []( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint32_t color, float thick ) {
      Draw->AddQuad( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImVec2( x4, y4 ), __builtin_bswap32( color ), thick );
    } );
    //  quadfill                 (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col);
    draw.set_function( "quadfill", []( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint32_t color ) {
      Draw->AddQuadFilled( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImVec2( x4, y4 ), __builtin_bswap32( color ) );
    } );
    //  triangle                 (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.0f);
    draw.set_function( "triangle", []( int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color, float thick ) {
      Draw->AddTriangle( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), __builtin_bswap32( color ), thick );
    } );
    //  trianglefill             (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);
    draw.set_function( "trianglefill", []( int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color ) {
      Draw->AddTriangleFilled( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), __builtin_bswap32( color ) );
    } );
    //  circle                   (const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
    draw.set_function( "circle", []( int cx, int cy, float radius, uint32_t color, float thickness ) {
      Draw->AddCircle( ImVec2( cx, cy ), radius, __builtin_bswap32( color ), 0, thickness );
    } );
    //  circlefill               (const ImVec2& center, float radius, ImU32 col, int num_segments = 0);
    draw.set_function( "circlefill", []( int cx, int cy, float radius, uint32_t color ) {
      Draw->AddCircleFilled( ImVec2( cx, cy ), radius, __builtin_bswap32( color ), 0 );
    } );
    //  ngon                     (const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness = 1.0f);
    draw.set_function( "ngon", []( int cx, int cy, float radius, uint32_t color, int segments, float thickness ) {
      Draw->AddNgon( ImVec2( cx, cy ), radius, __builtin_bswap32( color ), segments, thickness );
    } );
    //  ngonfill                 (const ImVec2& center, float radius, ImU32 col, int num_segments);
    draw.set_function( "ngonfill", []( int cx, int cy, float radius, uint32_t color, int segments ) {
      Draw->AddNgon( ImVec2( cx, cy ), radius, __builtin_bswap32( color ), segments );
    } );
    //  text                     (const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    draw.set_function( "text", []( int x, int y, uint32_t color, std::string text ) {
      Draw->AddText( ImVec2( x, y ), __builtin_bswap32( color ), text.c_str() );
    } );
    //  beziercurve              (const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments = 0);
    draw.set_function( "beziercurve", []( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint32_t color, float thick ) {
      Draw->AddBezierCurve( ImVec2( x1, y1 ), ImVec2( x2, y2 ), ImVec2( x3, y3 ), ImVec2( x4, y4 ), __builtin_bswap32( color ), thick, 0 );
    } );


    draw.set_function( "size", []( ) {
      int width, height;
      emscripten_get_canvas_element_size( "#canvas", &width, &height );
      return std::make_tuple( width, height );
    } );

    draw.set_function( "center", []( ) {
      int width, height;
      emscripten_get_canvas_element_size( "#canvas", &width, &height );
      return std::make_tuple( width / 2, height / 2 );
    } );
  }


};
