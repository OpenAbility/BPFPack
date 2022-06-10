#define BPF_IMPLEMENT
#include <bpf.h>

#include <fstream>
#include <dirent.h> 
#include <string>
#include <iostream>
#include <filesystem>
#include <string>
#include <streambuf>
#include <vector>

using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;


void print_file(bpf_file_t file){
    printf("VALID: %s\n", (bpf_verify_file(file) == BPF_TRUE ? "TRUE" : "FALSE"));
    printf("bpf_file_header: %s\n", file.header.bpf_file_header);
    printf("bpf_file_version: %i\n", file.header.bpf_file_version);
    printf("checksum: %i\n", file.header.checksum);
    printf("file_blocks: %i\n", file.header.file_blocks);
    printf("data_size: %i\n", file.header.data_size);
    for(int i = 0; i < file.header.file_blocks; i++) {
        printf("blocks[%i].file_name_hash: %i\n", i, file.blocks[i].file_name_hash);
        printf("blocks[%i].pointer_location: %i\n", i, file.blocks[i].pointer_location);
        printf("blocks[%i].pointer_size: %i\n", i, file.blocks[i].pointer_size);
    }
}

int main(int argc, char* argv[]){
    printf("-- BPFPACK --\n");

    char* directory;
    if(argc < 2){
        printf("ERROR: NO INPUT DIRECTORY\n");
        return -1;
    } else{
        directory = argv[1];
    }
    
    char* outfile;
    if(argc < 3){
        printf("ERROR: NO OUTPUT FILE; DEFAULTING TO DIRECTORY NAME!\n");
        outfile = (char*)malloc(strlen(directory));
        memcpy(outfile, directory, strlen(directory));
        strcat(outfile, ".bpf");
    } else{
        outfile = argv[2];
    }

    
    printf("Input directory: %s\n", directory);
    printf("Output file: %s\n", outfile);

    char* files[2048];
    int file_c = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_type==DT_REG){
                files[file_c] = (char*)malloc(256);
                memcpy(files[file_c], dir->d_name, 256);
                file_c++;
            }
        }
        closedir(d);
    } else{
        printf("ERROR: INVALID DIRECTORY\n");
        exit(EXIT_FAILURE);
    }
    
    printf("\nLISTING FILES:\n");
    for(int i = 0; i < file_c; i++){
        printf("\tPACK FILE\t %s\n", files[i]);
    }
    printf("LIST END\n");

    
    printf("Generating file...\n");
    unsigned int sizes[file_c];     // The sizes of the 2 resources data
    unsigned char* data[file_c]; // The complete data array

    for(int i = 0; i < file_c; i++){
        std::string path;
        path.append(directory).append("/").append(files[i]);
        printf("READING FILE\t");
        printf("%s\t", files[i]);

        ifstream in(path, std::ifstream::in | std::ifstream::binary);
        if(in.is_open()){
            std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in), {});
            sizes[i] = buffer.size();
            data[i] = (unsigned char*)malloc(buffer.size());
            memcpy(data[i], buffer.data(), buffer.size());

            printf("[OK]\n");
        } else{
            printf("[FAILED]\n");
            return 1;
        }
    }

    printf("Generating bpf_file_t...\n");

    bpf_file_t file = bpf_generate_file(files, file_c, sizes, data); // Generate the array

    printf("Generating raw data...\n");

    bpf_file_data_t filedata = bpf_serialize_file(file); // Serialize into a byte[];

    printf("Writing file...\n");

    // Write byte array to file
    ofstream stream;
    stream.open(outfile);
    if( !stream )
        cout << "Opening file failed" << endl;
    // use operator<< for clarity
    stream.write(filedata.data, filedata.size); // Filedata has the data segment and the size segment.
    // test if write was succesful - not *really* necessary
    if( !stream )
        cout << "Write failed!" << endl;

    printf("Task finished!\n");
    printf("Cleaning up...\n");

    for(int i = 0; i < file_c; i++){
        free(data[i]);
        free(files[i]);
    }

    return 0;
}
