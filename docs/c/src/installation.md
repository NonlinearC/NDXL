# Installation {#installation}

NDXL can be obtained from the library's <b>[GitHub repository](https://github.com/NonlinearC/NDXL/tree/main/NDXL)</b>.
The `C/C++` header files are located inside the `include` folder, and the precompiled libraries are located inside the `lib` folder.
Copy these folders directly into the working directory of your project, which we will assume has the following structure:

@dot
digraph FileStructure {
  rankdir=LR;
  ordering="out";
  node [shape=folder, style=filled, fillcolor="#eef4fa", fontname="Helvetica"];
  
  root [label="my-project/"];
  include [label="include/"];
  lib [label="lib/"];
  src [label="src/"];
  ndxl [label="ndxl/"];
  unicpp [label="unicpp/"];
  x64 [label="x64/"];
  unix [label="Unix/"];
  macos [label="macOS/"];
  x64static [label="Static/"];
  x64dynamic [label="Dynamic/"];
  unixstatic [label="Static/"];
  unixdynamic [label="Dynamic/"];
  macosstatic [label="Static/"];
  macosdynamic [label="Dynamic/"];
  
  node [shape=note, fillcolor="#ffffff"];
  ndxlh [label="ndxl.h"]
  ndxlcoreh [label="ndxlcore.h"]
  mainc [label="main.c"];
  channelh [label="ndxl_channel.h"];
  edgeh [label="ndxl_edge.h"];
  networkh [label="ndxl_network.h"];
  vertexh [label="ndxl_vertex.h"];
  unicppheaders [label="..."];
  x64lib [label="NDXL.lib"];
  x64lib2 [label="NDXL.lib"];
  x64dll [label="NDXL.dll"];
  unixlib [label="libNDXL.a"];
  unixdll [label="libNDXL.so"];
  maclib [label="libNDXL.a"];
  macdll [label="libNDXL.dylib"];

  root -> include;
  root -> lib;
  root -> src;
  
  include -> ndxl;
  include -> unicpp;
  include -> ndxlh;
  include -> ndxlcoreh;
  
  ndxl -> channelh;
  ndxl -> edgeh;
  ndxl -> networkh;
  ndxl -> vertexh;
  
  unicpp -> unicppheaders;
  
  lib -> x64;
  lib -> unix;
  lib -> macos;
  
  x64 -> x64static;
  x64 -> x64dynamic;
  
  unix -> unixstatic;
  unix -> unixdynamic;
  
  macos -> macosstatic;
  macos -> macosdynamic;
  
  x64static -> x64lib;
  x64dynamic -> x64lib2;
  x64dynamic -> x64dll;
  
  unixstatic -> unixlib;
  unixdynamic -> unixdll;
  
  macosstatic -> maclib;
  macosdynamic -> macdll;
  
  src -> mainc;
}
@enddot

In `main.c`, we will define a minimal working program calling @ref ndxlInit(), which <b>must</b> be called before using any NDXL functionality.

```c
#include "../include/ndxl.h"

int main(int argc, char** argv)
{
	ndxlInit();
	return 0;
}
```

We will cover here the installation and linking guides for both the static and dynamic libraries in `MSVC` and `gcc`.
Whether you use the static or dynamic version depends on you:
- Use the static version for faster runtime execution but a larger executable
- Use the dynamic version for a smaller executable but slower runtime execution

## Compiling with MSVC
In the solution explorer, right-click your project file, and click "Properties" at the very bottom.
At the top of the "Properties" dialogue, ensure the "Configuration" is set to "All Configurations", and "Platform" is set to "All Platforms".
On the left, click the "Linker" dropdown, then click on "Input".
At the top of the page, the "Additional Dependencies" field will likely contain:

```
$(CoreLibraryDependencies);%(AdditionalDependencies)
```

If your project uses other libraries, they will likely already be linked here too.
Add NDXL.lib to the end preceeded by a semicolon:

```
$(CoreLibraryDependencies);%(AdditionalDependencies);NDXL.lib
```

If the field is empty, simply set it to ```NDXL.lib```, then click onto another field so that Visual Studio processes the input.

### Linking against the static version (.lib)
Again under "Linker", navigate to "General", and search for the "Additional Library Directories" field.
If it is empty, simply paste:

```
$(ProjectDir)lib\x64\Static\
```

If the field contains next, click the drop-down arrow on the right, then click <Edit...>.
In the window that pops up click the icon showing a folder with a star ("New Line"), then paste the above text in that line, and click "OK".
Finally, click the "C/C++" dropdown on the left, and navigate to the "Preprocessor" tab.
Under "Preprocessor Definitions", add:

```
NDXL_STATIC
```

As before, if this field already contains text, append a semicolon before adding the above text.
Click "OK" in the bottom right of the "Properties" window, and, assuming your file structure is correctly reflected in MSVC, the static version of NDXL should be successfully linked.

### Linking against the dynamic version (.dll)
Repeat the first step of the prebvious section, but under "Additional Library Directories", instead paste:

```
$(ProjectDir)lib\x64\Dynamic\
```

<b>Do not</b> define the `NDXL_STATIC` preprocessor definition as it will attempt to import static symbols.
Finally, ensure that `NDXL.dll` is in the same path as your executable at runtime, otherwise you will run into a "dll not found error".
Either copy the file manually to your target directory, or automate the process using a [build event](https://learn.microsoft.com/en-us/cpp/build/how-to-use-build-events-in-msbuild-projects?view=msvc-170).

## Compiling with gcc
Ensure your terminal is currently in the root directory of your project:

```
user@pc:~$ cd path/to/my-project
```

Always remember to link against the standard math library `-lm`.

### Linking against the static version (.a)
In your terminal, type:

```
gcc -o my-project.out src/main.c -Llib/Unix/Static -lNDXL -lm
```

### Linking against the dynamic version (.so)
In your terminal, type:

```
gcc -o my-project.out src/main.c -Llib/Unix/Dynamic -lNDXL -lm
```

Ensure that `libNDXL.so` is discoverable by your runtime loader when you execute the program.
The easiest way to do this is to copy the file into the same directory as `my-project.out` and run the executable from that directory.

## References
For a detailed guide on linking libraries in `MSVC`, see the [MSVC documentation](https://learn.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-static-library-cpp?view=msvc-170).

For a detailed guide on linking libraries in `gcc`, see the [gcc website](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html).