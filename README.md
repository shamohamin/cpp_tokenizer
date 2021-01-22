# cpp_tokenizer

### start
```bash 
cd project_directory

# Put All Your Test Files Inside The Inputs Folder
# running with Python 
# (linux or unix)
python3 -m venv env
source env/bin/activate
python ui.py

# running cpp_src 
make && ./bin/main (place input file here) (place output file here) 
# its important input file and output file has same order like input1.txt and output1.txt
# Like
make && ./bin/main input1.txt output1.json

# running with make 
make run ARG1=input1.txt ARG2=output1.json 

