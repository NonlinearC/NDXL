# This build file is used by the NDXL maintainers to compile Unix, macOS, and non-MSVC static versions of NDXL.
# Use at your own risk.

import os
import platform
import re

srcdir = "src"
srcfiles = []

if platform.system() == "Windows":
	os.makedirs("lib/x64/Static", exist_ok=True)
	os.system("set LIB_STATIC=1")
	objfiles = []

	for root, _, filenames in os.walk(srcdir):
		for fname in filenames:
			if fname.endswith(".c"):
				srcfile = os.path.join(root, fname)
				srcfiles.append(srcfile)
				objfile = "bin-int/x64/Static/" + re.sub("\\.c", ".o", fname)
				objfiles.append(objfile)
				os.system(f"gcc -w -march=x86-64 -O3 -DNDEBUG -o {objfile} -c {srcfile}")

	# Build non-MSVC static library
	os.system(f"ar rcs lib/x64/Static/libNDXL.a {' '.join(objfiles)}")
	os.system("set LIB_STATIC=")
else:
	os.makedirs("lib/Unix/Static", exist_ok=True)
	os.makedirs("lib/Unix/Dynamic", exist_ok=True)
	os.makedirs("lib/macOS/Static", exist_ok=True)
	os.makedirs("lib/macOS/Dynamic", exist_ok=True)
	os.makedirs("bin-int/Unix/Static", exist_ok=True)
	os.makedirs("bin-int/Unix/Dynamic", exist_ok=True)
	os.makedirs("bin-int/macOS/Static", exist_ok=True)
	os.makedirs("bin-int/macOS/Dynamic", exist_ok=True)
	
	for root, _, filenames in os.walk(srcdir):
		for fname in filenames:
			if fname.endswith(".c"):
				srcfile = os.path.join(root, fname)
				srcfiles.append(srcfile)
				objfile = re.sub("\\.c", ".o", fname)
				os.system(f"NDXL_STATIC=1 gcc -w -march=x86-64 -O3 -DNDEBUG -o bin-int/Unix/Static/{objfile} -c {srcfile}")
				os.system(f"gcc -w -march=x86-64 -O3 -DNDEBUG -o bin-int/Unix/Dynamic/{objfile} -c -fPIC -fvisibility=hidden {srcfile}")
				#os.system(f"zig cc -w -target aarch64-macos -o bin-int/macOS/Static/{objfile} -c {srcfile}")
				os.system(f"zig cc -w -target x86_64-macos -O3 -DNDEBUG -o bin-int/macOS/Static/{objfile} -c {srcfile}")
	
	# Build Unix libraries
	os.system("ar rcs lib/Unix/Static/libNDXL.a bin-int/Unix/Static/*.o")
	os.system("gcc -O3 -o lib/Unix/Dynamic/libNDXL.so -shared  bin-int/Unix/Dynamic/*.o")
	
	# Build macOS libraries
	os.system("zig ar rcs lib/macOS/Static/libNDXL.a bin-int/macOS/Static/*.o")
	#os.system("zig cc -target aarch64-macos -shared -fPIC " + " ".join(srcfiles) + " -o lib/macOS/Dynamic/libNDXL-silicon.dylib")
	os.system("zig cc -target x86_64-macos -O3 -DNDEBUG -shared -fPIC " + " ".join(srcfiles) + " -o lib/macOS/Dynamic/libNDXL-intel.dylib")
	
	# Combine macOS libraries
	#os.system("llvm-lipo-18 -create lib/macOS/Dynamic/libNDXL-silicon.dylib lib/macOS/Dynamic/libNDXL-intel.dylib -output lib/macOS/Dynamic/libNDXL.dylib")
	os.system("llvm-lipo-18 -create lib/macOS/Dynamic/libNDXL-intel.dylib -output lib/macOS/Dynamic/libNDXL.dylib")