import glob
env = Environment(CCFLAGS=['-pthread','-Wall'])

sources = glob.glob("src/*.c");
env.Program(target="zos-fat",source=sources,LIBS=['pthread'])
