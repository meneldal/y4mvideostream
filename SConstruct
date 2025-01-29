#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")


env.Append(CPPPATH=["src"])
sources = Glob("src/*.cpp")


library = env.SharedLibrary(
        "demo/bin/y4mvideostream{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
)

Default(library)
