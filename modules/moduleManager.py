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




makefile1 = """all: directories lib/replicators.so

directories:
	mkdir -p build; mkdir -p lib

lib/replicators.so: build/modelHandler.o """

makefile2 = """build/BaseReplicator.o build/BaseEnvironment.o
	g++ -O3 -shared $^ -o $@

build/modelHandler.o: ./src/modelHandler.cpp ./include/modelHandler.h """

makefile3 = """../include/config.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ """

makefile4 = """-fpic -c $< -o $@

"""

makefile5 = """build/BaseReplicator.o: ./src/BaseReplicator.cpp ./include/BaseReplicator.h ../include/config.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@

build/BaseEnvironment.o: ./src/BaseEnvironment.cpp ./include/BaseEnvironment.h ../include/config.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@

clean:
	rm -f *.o *.so *~ \#* ./build/*.o ./lib/*.so
"""

makefileLine = """
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@

"""

modelHandler1 = """using namespace std;
#include <iostream>
#include "modelHandler.h"

// Replicators
"""

modelHandler2 = """
// Environments
"""

modelHandler3 = """
BaseReplicator* newModel(config &args)
{
  BaseReplicator *m;

  if (args.model=="BaseReplicator")
    m = new BaseReplicator(args);
"""

modelHandler4 = """  else                         
    m = new BaseReplicator(args);

  return m;
}

void deleteModel(config &args, BaseReplicator* modelObject)
{
  if (args.model=="BaseReplicator")
    delete modelObject;
"""

modelHandler5 = """  else
    delete modelObject;
}

BaseEnvironment* newEnvironment(config &args)
{
  BaseEnvironment *e;

  if (args.environment=="BaseEnvironment")
    e = new BaseEnvironment(args);
"""

modelHandler6 = """  else
    e = new BaseEnvironment(args);

  return e;
}

void deleteEnvironment(config &args, BaseEnvironment* environmentObject)
{
  if (args.environment=="BaseEnvironment")
    delete environmentObject;
"""

modelHandler7 = """  else
    delete environmentObject;
}
"""



stateText = ["What to do?\n\n[1] - add a module\n[2] - remove a module\n[3] - `recompile'\n[4] - exit\n\n", "Add a module\n\n[1] - reproduction module\n[2] - environment module\n[3] - back\n[4] - exit\n\n", "Remove a module\n\n[1] - reproduction module\n[2] - environment module\n[3] - back\n[4] - exit\n\n", "Add a reproduction module\n\nEnter module name: ", "Add an environment module\n\nEnter module name: ", "Remove a reproduction module\n\n", "Remove an environment module\n\n"]

reproductionModules = []
with open('./replicators/replicators', 'r') as replicatorsFile:
    reproductionModules = [line.rstrip('\n') for line in replicatorsFile.readlines()]
environmentModules = []
with open('./environments/environments', 'r') as environmentsFile:
    environmentModules = [line.rstrip('\n') for line in environmentsFile.readlines()]

def recompile():

    # Rewrite makefile
    makefileString = makefile1
    for module in reproductionModules:
        makefileString += "build/" + module + ".o "
    for module in environmentModules:
        makefileString += "build/" + module + ".o "
    makefileString += makefile2
    for module in reproductionModules:
        makefileString += "./replicators/" + module + "/" + module + ".h "
    for module in environmentModules:
        makefileString += "./environments/" + module + "/" + module + ".h "
    makefileString += makefile3
    for module in reproductionModules:
        makefileString += "-I./replicators/" + module + " "
    for module in environmentModules:
        makefileString += "-I./environments/" + module + " "
    makefileString += makefile4
    for module in reproductionModules:
        makefileString += "build/" + module + ".o: ./replicators/" + module + "/" + module + ".cpp ./replicators/" + module + "/" + module + ".h ./include/BaseReplicator.h"
        makefileString += makefileLine
    for module in environmentModules:
        makefileString += "build/" + module + ".o: ./environments/" + module + "/" + module + ".cpp ./environments/" + module + "/" + module + ".h ./include/BaseEnvironment.h"
        makefileString += makefileLine
    makefileString += makefile5

    with open('Makefile', 'w') as makefileFile:
        makefileFile.write(makefileString)

    # Rewrite modelHandler
    modelHandlerString = modelHandler1
    for module in reproductionModules:
        modelHandlerString += "#include \"" + module + ".h\"\n"
    modelHandlerString += modelHandler2
    for module in environmentModules:
        modelHandlerString += "#include \"" + module + ".h\"\n"
    modelHandlerString += modelHandler3
    for module in reproductionModules:
        modelHandlerString += "  else if (args.model==\"" + module + "\")\n    m = new " + module + "(args);\n"
    modelHandlerString += modelHandler4
    for module in reproductionModules:
        modelHandlerString += "  else if (args.model==\"" + module + "\")\n    delete ((" + module + "*)modelObject);\n"
    modelHandlerString += modelHandler5
    for module in environmentModules:
        modelHandlerString += "  else if (args.environment==\"" + module + "\")\n    e = new " + module + "(args);\n"
    modelHandlerString += modelHandler6
    for module in environmentModules:
        modelHandlerString += "  else if (args.environment==\"" + module + "\")\n    delete ((" + module + "*)environmentObject);\n"
    modelHandlerString += modelHandler7

    with open('src/modelHandler.cpp', 'w') as modelHandlerFile:
        modelHandlerFile.write(modelHandlerString)

    # Rewrite model list files
    with open('replicators/replicators', 'w') as repModuleFile:
        first = True
        for module in reproductionModules:
            if first:
                first = False
            else:
                repModuleFile.write("\n")
            repModuleFile.write(module)
    with open('environments/environments', 'w') as envModuleFile:
        first = True
        for module in environmentModules:
            if first:
                first = False
            else:
                envModuleFile.write("\n")
            envModuleFile.write(module)

    # If the directory doesn't exist, create it.
    # If the .h and .cpp files don't exist, create them
    for module in reproductionModules:
        if not os.path.exists("replicators/" + module):
            os.makedirs("replicators/" + module)
        if not os.path.isfile("replicators/" + module + "/" + module + ".h"):
            with open("replicators/" + module + "/" + module + ".h", 'w') as hFile:
                hFile.write("#include \"BaseReplicator.h\"\n\nclass ")
                hFile.write(module + " : public BaseReplicator\n{\npublic:\n  ")
                hFile.write(module + "(config &args);\n  ~" + module + "();\n};\n")
            with open("replicators/" + module + "/" + module + ".cpp", 'w') as cppFile:
                cppFile.write("using namespace std;\n#include <iostream>\n#include \"" + module + ".h\"\n\n")
                cppFile.write(module + "::" + module + "(config &args) : BaseReplicator(args)\n{\n}\n\n")
                cppFile.write(module + "::~" + module + "()\n{\n}\n")

    for module in environmentModules:
        if not os.path.exists("environments/" + module):
            os.makedirs("environments/" + module)
        if not os.path.isfile("environments/" + module + "/" + module + ".h"):
            with open("environments/" + module + "/" + module + ".h", 'w') as hFile:
                pass
                hFile.write("#include \"BaseEnvironment.h\"\n\nclass ")
                hFile.write(module + " : public BaseEnvironment\n{\npublic:\n\n")
                hFile.write("  void interpretBody(config&,int,int,int);\n  void updateFitnesses(config&);\n\n\n  " + module + "(config&);\n  ~" + module + "();\n\nprivate:\n\n};\n")
            with open("environments/" + module + "/" + module + ".cpp", 'w') as cppFile:
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
            if newRep in reproductionModules:
                postClearString = "Reproduction module called \"" + newRep + "\" already exists\n\n"
            else:
                reproductionModules.append(newRep)
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
