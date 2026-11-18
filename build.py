import os
import re

srcdir = "src"
os.makedirs("bin/Linux/Static", exist_ok=True)
os.makedirs("bin/Linux/Dynamic", exist_ok=True)
os.makedirs("bin-int/Linux/Static", exist_ok=True)
os.makedirs("bin-int/Linux/Dynamic", exist_ok=True)

for root, _, filenames in os.walk(srcdir):
	for fname in filenames:
		if fname.endswith(".c"):
			srcfile = os.path.join(root, fname)
			objfile = re.sub("\\.c", ".o", fname)
			os.system(f"gcc -w -march=x86-64 -o bin-int/Linux/Static/{objfile} -c {srcfile}")
			os.system(f"gcc -w -march=x86-64 -o bin-int/Linux/Dynamic/{objfile} -c -fPIC -fvisibility=hidden {srcfile}")

os.system("ar rcs bin/Linux/Static/libNDXL.a bin-int/Linux/Static/*.o")
os.system("gcc -o bin/Linux/Dynamic/libNDXL.so -shared  bin-int/Linux/Dynamic/*.o")
os.system("cp bin/Linux/Dynamic/libNDXL.so ../Bindings/Python/src/ndxl/lib/libNDXL.so")