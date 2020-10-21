
namespace Site{

  void Init();
  void Tick();

}

/*

  ImGui::Begin( "lua script", &Demo,  window );
  {

    static bool autorun = true;
    static int instructions = 40960;

    ImGui::Checkbox( "Auto update", &autorun );
    ImGui::SetNextItemWidth( -1 );
    ImGui::InputScalar( "Instruction budget", ImGuiDataType_U32, &instructions, nullptr, nullptr, nullptr );

    if( ImGui::Button( "Run", {-1, ImGui::GetTextLineHeightWithSpacing() } ) || autorun && runnable.valid() ) {
      output.clear();


      // running the script
      output_err = result;

      if( result.valid() && result.return_count() ) {
        output.append( "The script returned:\n" );

        for( auto var = result.begin() ; var != result.end(); var++ ) {
          output += var->get<std::string>() + '\n';
        }

      }
    }
    //the good


    if( output.size() ) {
      int lines = std::count( output.begin(), output.end(), '\n' );
      ImGui::InputTextMultiline( "#Output", output.data(), output.size(), ImVec2( -1, ImGui::GetTextLineHeightWithSpacing() * lines ), ImGuiInputTextFlags_ReadOnly );
    }

    // the ugly
    if( script_err.has_error() ) {
      ImGui::Text( "%s", script_err.what() );
    }

    if( output_err.has_error() ) {
      ImGui::Text( "%s", output_err.what() );
    }
  }
  ImGui::End();

  */
