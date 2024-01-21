# This is the clui part of the project

import sys
import os
import yaml

def main():
    # Checking for the correct number of arguments
    if len(sys.argv) != 2:
        print("Wrong number of arguments. Provide an input file name as \'filename.yml\'.")
        return
    # Check if provided filename exists, then open it
    if not os.path.isfile(str(sys.argv[1])):
        print("Cannot find specified file.")
        print(str(sys.argv[1]))
        return
    inFile = open(sys.argv[1],'rt')
    request = yaml.safe_load(inFile.read(-1))
    inFile.close()
    # Do things
    print(request)



if __name__ == "__main__":
    main()
