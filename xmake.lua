-- Imgui and lua on the web
set_plat("wasm")
-- with emscripten
set_languages("c++17")

-- runtime
add_cxflags("-O2 -s WASM=1 -sNO_FILESYSTEM=1 -sFILESYSTEM=0 -sALLOW_MEMORY_GROWTH=1 -sDISABLE_EXCEPTION_CATCHING=0 -sNO_EXIT_RUNTIME=0")
add_ldflags("-O2 -s WASM=1 -sNO_FILESYSTEM=1 -sFILESYSTEM=0 -sALLOW_MEMORY_GROWTH=1 -sDISABLE_EXCEPTION_CATCHING=0 -sNO_EXIT_RUNTIME=0")

target("editor")
	set_kind("static")
	add_files("./src/editor/*.cpp")

target("lua")
	set_kind("static")
	add_files("./src/lua/*.cpp")
	add_defines("LUA_USE_LINUX")

target("imgui")
	set_kind("static")
	add_files("./src/imgui/*.cpp")

	add_defines("DIMGUI_DISABLE_FILE_FUNCTIONS")
	add_cxflags("-s USE_SDL=2")
	add_ldflags("-s USE_SDL=2")

target("site")
	set_kind("static")
	add_files("./src/site/*.cpp")

target("toy")
	set_kind("binary")
	add_files("./src/*.cpp")

	-- lua fun
	add_deps("lua")

	-- imgui for a gui and lazy mans renderer
	add_deps("imgui")

	-- editor widget
	add_deps("editor")

	-- site logic
	add_deps("site")

	--html file to embed in
	add_ldflags("--shell-file ./src/shell_minimal.html")

	-- SDL2
	add_cxflags("-sUSE_SDL=2")
	add_ldflags("-sUSE_SDL=2")

	on_run(function(target)
		os.execv("emrun", {target:targetfile()})
	end)

--[[O-O]]--
