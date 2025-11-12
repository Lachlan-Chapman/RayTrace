import os
import subprocess
from pathlib import Path

SRC_DIRECTORY = Path("src")
INCLUDE_DIRECTORY = Path("include")
OBJ_DIRECTORY = Path("object")
BUILD_DIRECTORY = Path("build")

EXECUTABLE = BUILD_DIRECTORY / "app"

CPP_FLAGS = ["-std=c++20", "-O0", "-g", "-Iinclude"]
LINK_FLAGS = []

OBJ_DIRECTORY.mkdir(exist_ok = True) #if they dont exist well make em but i should have made em already
BUILD_DIRECTORY.mkdir(exist_ok = True)

cpp_files = list(SRC_DIRECTORY.glob(("*.cpp"))) #grab all files with .cpp prefix ONLY in src folder

obj_files = [] #empty arr to store all object files
for file in cpp_files:
	obj = OBJ_DIRECTORY / (file.stem + ".o") #just to grab the name and make equal object file
	obj_files.append(str(obj))
	print(f"--- compiling {file} -> {obj} ---")
	try:
		subprocess.run(["g++", "-c", *CPP_FLAGS, str(file), "-o", str(obj)], check = True)
	except subprocess.CalledProcessError:
		print(f"!!! Failed to compile [{file.name}] !!!")
		exit(1)

print(f"*** linking -> {EXECUTABLE}")
subprocess.run(["g++", *obj_files, "-o", str(EXECUTABLE), *LINK_FLAGS], check = True)

print(f"\n >>> Built [{EXECUTABLE}] Complete <<<")
