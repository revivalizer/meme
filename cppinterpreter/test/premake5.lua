local invader = {}
invader.commontargetdir = "build/%{prj.name}/bin/%{cfg.longname}"
invader.commonobjdir    = "build/%{prj.name}/obj/%{cfg.longname}"

invader.commoncompilerflags   = { "FatalWarnings", "MultiProcessorCompile", "No64BitChecks",  "NoEditAndContinue" }
-- not sure about "NoFramePointer"
invader.commonstandaloneflags = { "NoBufferSecurityCheck", "NoExceptions", "NoManifest", "NoRTTI", "NoRuntimeChecks", "OmitDefaultLibrary" }
invader.commonbuildoptions    = { "/wd4725 /wd4201" } -- 4725 is old warning for tan asm func
invader.commonlinkoptions     = { "/NODEFAULTLIB" }

solution "memetest"
	configurations { "Debug", "Release", "Debug Standalone", "Release Standalone", "Release Standalone Crinkler", "Release Standalone Kkrunchy" }

	targetdir(invader.commontargetdir)
	objdir   (invader.commonobjdir)

	flags(invader.commoncompilerflags)
	flags(invader.commonstandaloneflags)
--	removeflags { "OmitDefaultLibrary" }

	buildoptions(invader.commonbuildoptions)
	linkoptions (invader.commonlinkoptions)
--	removelinkoptions { "/NODEFAULTLIB" }

	warnings       "extra"
	floatingpoint  "fast"

	defines { "_CRT_SECURE_NO_WARNINGS" }

	includedirs { "../../../64klibs" }
	includedirs { "$(SolutionDir)/.." } -- for config.h

	location "build"

	filter "configurations:*Standalone"
		defines { "STANDALONE" }
	filter {}

	project "test"
		kind "WindowedApp"
		targetname "test"

		pchheader "pch.h"
		pchsource "pch.cpp"

		files { "*.h", "*.cpp" }
		files { "config.h" }
		files { "notes.txt" }
		files { "unittests/*" }

		links { "base" }
		links { "startup" }
		links { "meme" }

		filter "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "Maps" }
			optimize "Off"

		filter "Debug Standalone"
			defines { "DEBUG" }
			flags { "Symbols", "Maps" }
			optimize "Off"
			defines { "STANDALONE" }

		filter "Release"
			defines { "NDEBUG" }
			optimize "Size"

		filter "Release Standalone"
			defines { "NDEBUG" }
			optimize "Size"
			defines { "STANDALONE" }

		filter "Release Standalone Crinkler"
			defines { "NDEBUG" }
			optimize "Size"
			defines { "STANDALONE" }
			linkoptions {"/CRINKLER /REPORT:crinkler.html /TRANSFORM:CALLS"}

		filter "Release Standalone Kkrunchy"
			defines { "NDEBUG" }
			optimize "Size"
			defines { "STANDALONE" }
			-- needs post processing kkrunchy call

	include "../../../64klibs/base"
	include "../../../64klibs/startup"
	include ".."
