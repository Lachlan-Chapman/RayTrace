import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import List, Tuple

SRC_DIRECTORY = Path("src")
INCLUDE_DIRECTORY = Path("include")
OBJ_DIRECTORY = Path("object")
BUILD_DIRECTORY = Path("build")

EXECUTABLE = BUILD_DIRECTORY / "app"

def getGitHash():
	try:
		hash = subprocess.run(
			["git", "rev-parse", "--short", "HEAD"],
			capture_output = True,
			text = True,
			check = True
		)
		return hash.stdout.strip()
	except:
		return "idk"

CXX = "g++"
CPP_FLAGS = ["-std=c++20", f"-DGIT_HASH=\"{getGitHash()}\"", "-O0", "-g", "-Iinclude",]
LINK_FLAGS = []

def verifyDirectory():
	OBJ_DIRECTORY.mkdir(exist_ok = True) #if they dont exist well make em but i should have made em already
	BUILD_DIRECTORY.mkdir(exist_ok = True)

def getObjectFiles():
	return [str(_path) for _path in OBJ_DIRECTORY.glob("*.o")]

def getMissingObject(): #get cpp files without any associated object
	stale = []
	for src in SRC_DIRECTORY.rglob("*.cpp"):
		obj = OBJ_DIRECTORY / (src.stem + ".o") #does object file exist for cpp
		if(not obj.exists()):
			print(f"missing object {src}")
			stale.append((src, obj)) #pair to compile
	return stale

def getMissingDependency(): #get object files without dependency
	stale = []
	for src in SRC_DIRECTORY.rglob("*.cpp"):
		dep = OBJ_DIRECTORY / (src.stem + ".d")
		obj = OBJ_DIRECTORY / (src.stem + ".o")
		if(not dep.exists()):
			print(f"missing dependency {src}")
			stale.append((src, obj))
	return stale

def getModifiedCpp(): #get cpp files that have changed since last object compilation
	stale = []
	for src in SRC_DIRECTORY.rglob("*.cpp"):
		dep = OBJ_DIRECTORY / (src.stem + ".o")
		obj = OBJ_DIRECTORY / (src.stem + ".o")
		if obj.exists() and src.stat().st_mtime > obj.stat().st_mtime:
			print(f"modified cpp {src}")
			stale.append((src, obj))
	return stale

def parseDependency(dep_file): #a dependency not mentioning a new hpp include
	try:
		text = dep_file.read_text().replace("\\", "") #just taking out the line continuation symbol in the .d file
	except:
		return [] #could be file doesnt exist etc
	parts = text.split(":") #grab the fiels listed after the .o file of the .hpp files that .o relies on
	if len(parts) < 2: #should be the .o name in [0] and then the .hpp files in [1]
		return []
	deps = parts[1].strip().split() #return array of .hpp files
	return deps

def getModifiedDependency():
	stale = []
	for src in SRC_DIRECTORY.rglob("*.cpp"):
		obj = OBJ_DIRECTORY / (src.stem + ".o")
		dep = OBJ_DIRECTORY / (src.stem + ".d")

		if not obj.exists() or not dep.exists():
			continue #another func handles this

		deps = parseDependency(dep)
		obj_time = obj.stat().st_mtime

		for _dep in deps:
			_path = Path(_dep)
			if _path.exists() and _path.stat().st_mtime > obj_time:
				print(f"modified dependency {src}")
				stale.append((src, obj))
				break
	return stale

def getStaleFiles():
	all_stale = (
		getMissingObject() +
		getMissingDependency() +
		getModifiedCpp() +
		getModifiedDependency()
	)

	unique = {}
	for src, obj in all_stale:
		unique[str(src)] = (src, obj)

	return list(unique.values())

def compileST(pairs: (Path, Path)):
	obj_files = [] #empty arr to store all the new object files

	for src, obj in pairs:
		print(f"-- Compiling {src} -> {obj} --")
		try:
			subprocess.run([CXX, "-c", "-MMD", "-MP", *CPP_FLAGS, str(src), "-o", str(obj)], check = True)
		except subprocess.CalledProcessError:
			print(f"\n++!- Failed to compile [{src.name}] -!++")
			obj.unlink(missing_ok = True) #delete the stale obj file if compilation fail
			raise SystemExit(1)
		obj_files.append(str(obj))
	return obj_files

def compileWorker(src: Path, obj: Path):
	cmd = [CXX, "-c", "-MMD", "-MP", *CPP_FLAGS, str(src), "-o", str(obj)]
	print(f"-- Compiling {src} -> {obj} --")
	try:
		subprocess.run(cmd, check = True)
		return None
	except subprocess.CalledProcessError as _err:
		return _err


def compileMT(pairs):
	with ThreadPoolExecutor(max_workers = 8) as pool:
		futures = [pool.submit(compileWorker, src, obj) for (src, obj) in pairs] #run func for each item in pairs
		for _fut in as_completed(futures):
			_err = _fut.result()
			if _err is not None:
				print(f"\n=== Failed To Compile {pairs[0][0]} ===")
				print(_err)
				print(f"\n=== Failed To Compile ===\n\n")
				for f in futures:
					if not f.done():
						f.cancel() #dont bother compiling any more, the error is probably going to permiate.
				return False
	
	print("Compiling Complete")
	return True


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
	pairs = getStaleFiles() #set of all cpp files that have some reason to compile such as no .o file, changed .hpp file or new/removes #includes
	if not pairs:
		print(f"++ No Changes | Skipping Build ++")
	else:
		useMultiThread = True
		if(useMultiThread):
			if (not compileMT(pairs)): #compile whats new
				exit(1)
		else:
			compileST(pairs)
	
	obj_files = getObjectFiles()
	link(obj_files) #link if needed
	

if __name__ == "__main__":
	main() #run main on run
