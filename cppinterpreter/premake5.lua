local invader = {}
invader.commontargetdir = "build/%{prj.name}/bin/%{cfg.longname}"
invader.commonobjdir    = "build/%{prj.name}/obj/%{cfg.longname}"

invader.commoncompilerflags   = { "FatalWarnings", "MultiProcessorCompile", "No64BitChecks",  "NoEditAndContinue" }
-- not sure about "NoFramePointer"
invader.commonstandaloneflags = { "NoBufferSecurityCheck", "NoExceptions", "NoManifest", "NoRTTI", "NoRuntimeChecks", "OmitDefaultLibrary" }
invader.commonbuildoptions    = { "/wd4725" } -- 4725 is old warning for tan asm func
invader.commonlinkoptions     = { "/NODEFAULTLIB" } -- 4725 is old warning for tan asm func

solution "memecpp"
	configurations { "Debug", "Release" }

	targetdir(invader.commontargetdir)
	objdir   (invader.commonobjdir)

	flags(invader.commoncompilerflags)
	flags(invader.commonstandaloneflags)
--	removeflags { "OmitDefaultLibrary" }

	buildoptions(invader.commonbuildoptions)
	linkoptions (invader.commonlinkoptions)
--	removelinkoptions { "/NODEFAULTLIB" }
	linkoptions {"/CRINKLER /REPORT:crinkler.html /TRANSFORM:CALLS"}

	warnings       "extra"
	floatingpoint  "fast"

	defines { "_CRT_SECURE_NO_WARNINGS" }

	includedirs { "../../trespasser" }

	location "build"

	project "memecpp"
		kind "WindowedApp"
		targetname "lift"

		files { "*.h", "*.cpp" }

		links { "base" }
		links { "startup" }

		filter "Debug"
			defines { "DEBUG" }
			flags { "Symbols", "Maps" }
			optimize "Off"

		filter "Release"
			defines { "NDEBUG" }
			optimize "Size"

	include "../../trespasser/libs/base"
	include "../../trespasser/libs/startup"
