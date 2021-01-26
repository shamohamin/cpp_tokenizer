# cpp_tokenizer

### start
```bash 
cd project_directory

# Put All Your Test Files Inside The Inputs Folder
# running with Python 
# (linux or unix)
# with running python script you dont need to specify the inputs it will scan all input files inside the inputs Fold
python3 -m venv env
source env/bin/activate
python ui.py

# running cpp_src 
# in this mode you must specify the input and output file. input file must be inside inputs Folder
make && ./bin/main (place input file here) (place output file here) 
# its important input file and output file has same order like input1.txt and output1.txt
# Like
# you need to specify name of file inside the inputs folder cpp will do the rest 
# note that output file must have json extension
make && ./bin/main input1.txt output1.json

# running with make 
make run ARG1=input1.txt ARG2=output1.json 

