#include "site.h"

#include "../imgui/imgui.h"
#include "../sol/sol.h"
#include "../editor/TextEditor.h"
#include "background.h"

namespace Site {

  sol::state Lua;
  TextEditor Editor;
  std::string output;

  void Init() {
    Lua.open_libraries();
    Editor.SetPalette( TextEditor::GetLightPalette() );

    Editor.SetText( std::string(
                      "start = os.clock()\n"
                      "\n"
                      "function rainbow(time)\n"
                      "\ttime = time * 6\n"
                      "\tred   = math.floor( math.sin(time+2) * 127 + 128)\n"
                      "\tgreen = math.floor( math.sin(time+0) * 127 + 128)\n"
                      "\tblue  = math.floor( math.sin(time+4) * 127 + 128)\n"
                      "\n"
                      "\treturn red << 24 | green << 16 | blue << 8 | 0xff\n"
                      "end\n"
                      "\n"
                      "cx, cy = draw.center()\n"
                      "\n"
                      "for r = 20, cy, 4 do\n"
                      "\tdraw.circle(cx, cy, r, rainbow(start+r), 4)\n"
                      "end\n"
                      "\n"
                      "for t = start, start+87 do\n"
                      "\txx = math.floor(cx + math.cos(t) * cy )\n"
                      "\tyy = math.floor(cy + math.sin(t) * cy )\n"
                      "\tdraw.line(cx, cy, xx, yy, rainbow(t), 3)\n"
                      "end\n"
                    ) );
    sol::table draw = Lua["draw"].get_or_create<sol::table>();
    Background::LuaBindings( draw );

    // overwrite print to have in window output
    Lua.set_function( "print", []( sol::variadic_args va ) {
      for( auto var : va ) {
        output += var.get<std::string>();
      }
    } );

    sol::table util = Lua["util"].get_or_create<sol::table>();

		util.set_function( "delta", []() {
      return ImGui::GetIO().DeltaTime;
    } );

  }

  class splitter : public std::string {};

  std::istream &operator>>( std::istream &is, splitter &output ) {
    std::getline( is, output, ':' );
    return is;
  }

  void lua_error( std::string error ) {

    std::istringstream iss( error );
    std::vector<std::string> tokens(
      ( std::istream_iterator<splitter>( iss ) ),
      std::istream_iterator<splitter>()
    );

    for( auto token : tokens ) {
      if( std::all_of( token.begin(), token.end(), isdigit ) ) {
        Editor.SetErrorMarkers( { { std::stoi( token ), error} } );
      }
    }
  }

  void Script() {
    sol::load_result runnable = Lua.load( Editor.GetText() );

    if( !runnable.valid() ) {
      lua_error( sol::error( runnable ).what() );
    }

    auto timeout = []( lua_State * L, lua_Debug * ) {
      luaL_error( L, "Out of allotted instructions!" );
    };

    lua_sethook( Lua.lua_state(), timeout, LUA_MASKCOUNT, 40960 );

    sol::safe_function_result result = runnable();

    lua_sethook( Lua.lua_state(), timeout, 0, 0 );

    if( !result.valid() ) {
      lua_error( sol::error( result ).what() );
    }

    if( result.return_count() ) {
      output = "The script returned:\n";

      for( auto var = result.begin() ; var != result.end(); var++ ) {
        output += var->get<std::string>() + '\n';
      }
    }
  }

  void Tick() {
    Background::Begin();

    Script();

    ImGui::SetNextWindowPos( ImVec2( 20, 20 ), ImGuiCond_Once );
    ImGui::SetNextWindowSize( ImVec2( 600, 600 ), ImGuiCond_Once );

    if( ImGui::Begin( "Editor" ) ) {
      Editor.Render( "#Editor", ImVec2( -1, ImGui::GetTextLineHeight() * 32 ) );
      ImGui::InputTextMultiline( "#output", output.data(), output.length(), ImVec2( -1, -1 ), ImGuiInputTextFlags_ReadOnly );
      Editor.SetErrorMarkers( { } );
    }

    output.clear();

    ImGui::End();

    Background::End();
  }

}
