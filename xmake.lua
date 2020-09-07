-- Imgui on the web
set_toolchains("emcc")
-- SDL2
add_cxflags("-s USE_SDL=2")
add_ldflags("-s USE_SDL=2")
-- runtime
add_cxflags("-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1")
add_ldflags("-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1")
--no fonts or files
add_cxflags("-s NO_FILESYSTEM=1 -DIMGUI_DISABLE_FILE_FUNCTIONS")
add_ldflags("-s NO_FILESYSTEM=1 -DIMGUI_DISABLE_FILE_FUNCTIONS")

target("demo.html")
	set_kind("binary")
	add_files("./src/*.cpp")
	add_files("./src/imgui/*.cpp")
	add_ldflags("--shell-file ./src/shell_minimal.html")

	on_run(function(target)
		os.execv("emrun", {target:targetfile()})
	end)

--[[O-O]]--
