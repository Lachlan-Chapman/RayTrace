import subprocess
from pathlib import Path

SRC_DIRECTORY = Path("src")
INCLUDE_DIRECTORY = Path("include")
OBJ_DIRECTORY = Path("object")
BUILD_DIRECTORY = Path("build")

EXECUTABLE = BUILD_DIRECTORY / "app"

CXX = "g++"
CPP_FLAGS = ["-std=c++20", "-O0", "-g", "-Iinclude"]
LINK_FLAGS = []

def verifyDirectory():
	OBJ_DIRECTORY.mkdir(exist_ok = True) #if they dont exist well make em but i should have made em already
	BUILD_DIRECTORY.mkdir(exist_ok = True)

def allObjectFiles():
	return [str(_path) for _path in OBJ_DIRECTORY.glob("*.o")]

def killOrphans():
	for obj in OBJ_DIRECTORY.glob("*.o"):
		src = SRC_DIRECTORY / (obj.stem + ".cpp")
		if not src.exists():
			obj.unlink(missing_ok = True)
			dependency = obj.with_suffix(".d") #to handle dependency files also
			dependency.unlink(missing_ok = True)

def getStale():
	stale = []
	for src in SRC_DIRECTORY.glob("*.cpp"):
		obj = OBJ_DIRECTORY / (src.stem + ".o")
		if (not obj.exists()) or (src.stat().st_mtime > obj.stat().st_mtime): #if there is no o file or the o file is older than the cpp file
			stale.append((src, obj))
	return stale

def compile(pairs):
	obj_files = [] #empty arr to store all the new object files

	for src, obj in pairs:
		print(f"-- compiling {src} -> {obj} --")
		try:
			subprocess.run([CXX, "-c", *CPP_FLAGS, str(src), "-o", str(obj)], check = True)
		except subprocess.CalledProcessError:
			print(f"\n++!- Failed to compile [{src.name}] -!++")
			obj.unlink(missing_ok = True) #delete the stale obj file if compilation fail
			raise SystemExit(1)
		obj_files.append(str(obj))
	return obj_files

def link(obj_files):
	if not obj_files: #no obj files were made so theres nothing new to link
		return
	if EXECUTABLE.exists():
		newest_obj = max(Path(_path).stat().st_mtime for _path in obj_files) #find the newest obj so we can see if rebuilding is needed
		if newest_obj < EXECUTABLE.stat().st_mtime:
			print(f"== cache linkage ==")
			print(f"\n>>> Built [{EXECUTABLE}] complete **")
			return

	print(f"** Linking -> [{EXECUTABLE}] **")
	subprocess.run([CXX, *obj_files, "-o", str(EXECUTABLE), *LINK_FLAGS], check=True)
	print(f"\n** Linking -> [{EXECUTABLE}] complete **")

def main():
	verifyDirectory()
	killOrphans() #clean o files with no counter cpp file
	pairs = getStale()
	all_obj = allObjectFiles()
	if not pairs:
		print(f"++ No Changes In .cpp | Skipping Build ++")
		link(all_obj) #link if needed
		return
	obj_changed = compile(pairs) #compile whats new
	link(sorted(set(obj_changed) | set(all_obj))) #get all o files

if __name__ == "__main__":
	main() #run main on run