import curses
import os

stdscr = curses.initscr()

curses.noecho()
curses.cbreak()
stdscr.keypad(1)

main   = 0
add    = 1
remove = 2
addRep = 3
addEnv = 4
remRep = 5
remEnv = 6
state  = main


makefile1 = """all: directories lib/modules.so

directories:
	mkdir -p build; mkdir -p lib

lib/modules.so: build/module_handler.o """

makefile2 = """build/BaseDevMechanism.o build/BaseEnvironment.o
	g++ -O3 -shared $^ -o $@

build/module_handler.o: ./src/module_handler.cpp ./include/module_handler.h """

makefile3 = """../include/config.h
	g++ -O3 -std=c++11 -I/home/andrew/local/boost_1_55_0/installed/include -I./include/ -I../include/ """

makefile4 = """-fpic -c $< -o $@

"""

makefile5 = """build/BaseDevMechanism.o: ./src/BaseDevMechanism.cpp ./include/BaseDevMechanism.h ../include/config.h
	g++ -O3 -std=c++11 -I/home/andrew/local/boost_1_55_0/installed/include  -I./include/ -I../include/ -fpic -c $< -o $@

build/BaseEnvironment.o: ./src/BaseEnvironment.cpp ./include/BaseEnvironment.h ../include/config.h
	g++ -O3 -std=c++11 -I/home/andrew/local/boost_1_55_0/installed/include -I./include/ -I../include/ -fpic -c $< -o $@

clean:
	rm -f *.o *.so *~ \#* ./build/*.o ./lib/*.so
"""

makefileLine = """
	g++ -O3 -std=c++11 -I/home/andrew/local/boost_1_55_0/installed/include -I./include/ -I../include/ -fpic -c $< -o $@

"""

moduleHandler1 = """using namespace std;
#include <iostream>
#include "module_handler.h"

// Development modules
"""

moduleHandler2 = """
// Environment modules
"""

moduleHandler3 = """
BaseDevMechanism* newOrganism(config &args)
{
  BaseDevMechanism *m;

  if (args.developmentMechanism=="BaseDevMechanism")
    m = new BaseDevMechanism(args);
"""

moduleHandler4 = """  else                         
    m = new BaseDevMechanism(args);

  return m;
}

void deleteOrganism(config &args, BaseDevMechanism* organismObject)
{
  if (args.developmentMechanism=="BaseDevMechanism")
    delete organismObject;
"""

moduleHandler5 = """  else
    delete organismObject;
}

BaseEnvironment* newEnvironment(config &args)
{
  BaseEnvironment *e;

  if (args.environment=="BaseEnvironment")
    e = new BaseEnvironment(args);
"""

moduleHandler6 = """  else
    e = new BaseEnvironment(args);

  return e;
}

void deleteEnvironment(config &args, BaseEnvironment* environmentObject)
{
  if (args.environment=="BaseEnvironment")
    delete environmentObject;
"""

moduleHandler7 = """  else
    delete environmentObject;
}
"""



stateText = ["What to do?\n\n[1] - add a module\n[2] - remove a module\n[3] - `recompile'\n[4] - exit\n\n", "Add a module\n\n[1] - development module\n[2] - environment module\n[3] - back\n[4] - exit\n\n", "Remove a module\n\n[1] - development module\n[2] - environment module\n[3] - back\n[4] - exit\n\n", "Add a reproduction module\n\nEnter module name: ", "Add an environment module\n\nEnter module name: ", "Remove a reproduction module\n\n", "Remove an environment module\n\n"]

developmentModules = []
with open('./development_modules/development_module_list', 'r') as developmentFile:
    developmentModules = [line.rstrip('\n') for line in developmentFile.readlines()]
environmentModules = []
with open('./environment_modules/environment_module_list', 'r') as environmentFile:
    environmentModules = [line.rstrip('\n') for line in environmentFile.readlines()]

def recompile():

    # Rewrite makefile
    makefileString = makefile1
    for module in developmentModules:
        makefileString += "build/" + module + ".o "
    for module in environmentModules:
        makefileString += "build/" + module + ".o "
    makefileString += makefile2
    for module in developmentModules:
        makefileString += "./development_modules/" + module + "/" + module + ".h "
    for module in environmentModules:
        makefileString += "./environment_modules/" + module + "/" + module + ".h "
    makefileString += makefile3
    for module in developmentModules:
        makefileString += "-I./development_modules/" + module + " "
    for module in environmentModules:
        makefileString += "-I./environment_modules/" + module + " "
    makefileString += makefile4
    for module in developmentModules:
        makefileString += "build/" + module + ".o: ./development_modules/" + module + "/" + module + ".cpp ./development_modules/" + module + "/" + module + ".h ./include/BaseDevMechanism.h"
        makefileString += makefileLine
    for module in environmentModules:
        makefileString += "build/" + module + ".o: ./environment_modules/" + module + "/" + module + ".cpp ./environment_modules/" + module + "/" + module + ".h ./include/BaseEnvironment.h"
        makefileString += makefileLine
    makefileString += makefile5

    with open('Makefile', 'w') as makefileFile:
        makefileFile.write(makefileString)

    # Rewrite moduleHandler
    moduleHandlerString = moduleHandler1
    for module in developmentModules:
        moduleHandlerString += "#include \"" + module + ".h\"\n"
    moduleHandlerString += moduleHandler2
    for module in environmentModules:
        moduleHandlerString += "#include \"" + module + ".h\"\n"
    moduleHandlerString += moduleHandler3
    for module in developmentModules:
        moduleHandlerString += "  else if (args.developmentMechanism==\"" + module + "\")\n    m = new " + module + "(args);\n"
    moduleHandlerString += moduleHandler4
    for module in developmentModules:
        moduleHandlerString += "  else if (args.developmentMechanism==\"" + module + "\")\n    delete ((" + module + "*)organismObject);\n"
    moduleHandlerString += moduleHandler5
    for module in environmentModules:
        moduleHandlerString += "  else if (args.environment==\"" + module + "\")\n    e = new " + module + "(args);\n"
    moduleHandlerString += moduleHandler6
    for module in environmentModules:
        moduleHandlerString += "  else if (args.environment==\"" + module + "\")\n    delete ((" + module + "*)environmentObject);\n"
    moduleHandlerString += moduleHandler7

    with open('src/module_handler.cpp', 'w') as moduleHandlerFile:
        moduleHandlerFile.write(moduleHandlerString)

    # Rewrite development mechanism list file
    with open('development_modules/development_module_list', 'w') as developmentFile:
        first = True
        for module in developmentModules:
            if first:
                first = False
            else:
                developmentFile.write("\n")
            developmentFile.write(module)
    with open('environment_modules/environment_module_list', 'w') as envModuleFile:
        first = True
        for module in environmentModules:
            if first:
                first = False
            else:
                envModuleFile.write("\n")
            envModuleFile.write(module)

    # If the directory doesn't exist, create it.
    # If the .h and .cpp files don't exist, create them
    for module in developmentModules:
        if not os.path.exists("development_modules/" + module):
            os.makedirs("development_modules/" + module)
        if not os.path.isfile("development_modules/" + module + "/" + module + ".h"):
            with open("development_modules/" + module + "/" + module + ".h", 'w') as hFile:
                hFile.write("#include \"BaseDevMechanism.h\"\n\nclass ")
                hFile.write(module + " : public BaseDevMechanism\n{\npublic:\n  ")
                hFile.write(module + "(config &args);\n  ~" + module + "();\n};\n")
            with open("development_modules/" + module + "/" + module + ".cpp", 'w') as cppFile:
                cppFile.write("using namespace std;\n#include <iostream>\n#include \"" + module + ".h\"\n\n")
                cppFile.write(module + "::" + module + "(config &args) : BaseDevMechanism(args)\n{\n}\n\n")
                cppFile.write(module + "::~" + module + "()\n{\n}\n")

    for module in environmentModules:
        if not os.path.exists("environment_modules/" + module):
            os.makedirs("environment_modules/" + module)
        if not os.path.isfile("environment_modules/" + module + "/" + module + ".h"):
            with open("environment_modules/" + module + "/" + module + ".h", 'w') as hFile:
                pass
                hFile.write("#include \"BaseEnvironment.h\"\n\nclass ")
                hFile.write(module + " : public BaseEnvironment\n{\npublic:\n\n")
                hFile.write("  void interpretBody(config&,int,int,int);\n  void updateFitnesses(config&);\n\n\n  " + module + "(config&);\n  ~" + module + "();\n\nprivate:\n\n};\n")
            with open("environment_modules/" + module + "/" + module + ".cpp", 'w') as cppFile:
                cppFile.write("using namespace std;\n#include <iostream>\n\n")
                cppFile.write("#include \"" + module + ".h\"\n\n// Given the coordinates of a new entity, create whatever we need to in order to evaluate its fitness immediately and/or in the future\n")
                cppFile.write("void " + module + "::interpretBody(config &args, int x, int y, int t)\n{\n}\n\n")
                cppFile.write("void " + module + "::updateFitnesses(config &args)\n{\n}\n\n")
                cppFile.write(module + "::" + module + "(config &args) : BaseEnvironment(args)\n{\n}\n\n" + module + "::~" + module + "()\n{\n}\n")


    return "Recompiled\n\n"

while 1:
    stdscr.addstr(stateText[state])
    stdscr.refresh()

    postClearString = ""

    if state == main:
        c = stdscr.getch()
        if c == ord("1"):
            state = add
        elif c == ord("2"):
            state = remove
        elif c == ord("3"):
            # todo recompile
            postClearString += recompile()
        elif c == ord("4"):
            break
    elif state == add:
        c = stdscr.getch()
        if c == ord("1"):
            state = addRep
        elif c == ord("2"):
            state = addEnv
        elif c == ord("3"):
            state = main
        elif c == ord("4"):
            break
    elif state == remove:
        c = stdscr.getch()
        if c == ord("1"):
            state = remRep
        elif c == ord("2"):
            state = remEnv
        elif c == ord("3"):
            state = main
        elif c == ord("4"):
            break
    elif state == addRep:
        curses.echo()
        newRep = stdscr.getstr()
        curses.noecho()
        stdscr.addstr("Add reproduction module \"" + newRep + "\"? [y/n]")
        ans = 0
        while not (ans == ord("y") or ans == ord("n")):
            ans = stdscr.getch()
        if ans == ord("y"):
            if newRep in developmentModules:
                postClearString = "Reproduction module called \"" + newRep + "\" already exists\n\n"
            else:
                developmentModules.append(newRep)
                postClearString = "Added reproduction module \"" + newRep + "\"\n\n"
                postClearString += recompile()
        state = main
    elif state == addEnv:
        curses.echo()
        newEnv = stdscr.getstr()
        curses.noecho()
        stdscr.addstr("Add environment module \"" + newEnv + "\"? [y/n]")
        ans = 0
        while not (ans == ord("y") or ans == ord("n")):
            ans = stdscr.getch()
        if ans == ord("y"):
            if newEnv in environmentModules:
                postClearString = "Environment module called \"" + newEnv + "\" already exists\n\n"
            else:
                environmentModules.append(newEnv)
                postClearString = "Added environment module \"" + newEnv + "\"\n\n"
                postClearString += recompile()
        state = main
    elif state == remRep:
        state = main
    elif state == remEnv:
        state = main

    stdscr.clear()

    stdscr.addstr(postClearString)

curses.nocbreak(); stdscr.keypad(0); curses.echo()
curses.endwin()
