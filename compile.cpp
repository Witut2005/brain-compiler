

#include <stdio.h>
#include <fstream>
#include <stdint.h>
#include <stack>
#include "./registers.h"

std::fstream file;
std::fstream compiled_file;

char code[500];
char buf;
char opcode;

bool in_loop = false;
uint8_t bytes_in_loop = 0x0;


uint8_t current_file_position = 0x0;

uint8_t index = 0x0;
uint8_t loop_offset[10] = {0x0};
std::stack<uint32_t> loops;


int main(int argc, char* argv[])
{

    if(argc != 2)
    {
        printf("usage: %s file_to_compile\n", argv[0]);
        exit(1);
    }

    
    file.open(argv[1], std::ios::in);
    
    if(file.good() == false)
    {
        printf("FILE ERROR\n");
        exit(1);
    }

    compiled_file.open("output.bin", std::ios::out | std::ios::binary);    
     
    int i = 0x0;

    while(file.read(&buf, sizeof(char)))
    {
        i++;

        switch(buf)
        {
            
            case '>':
            {   
                opcode = inc + si; 
                compiled_file.write(&opcode, sizeof(char));   
                
                current_file_position++;

                if(in_loop)
                    loop_offset[index]++;
                
                break;
            }

            case '<':
            {
                opcode = dec + si;
                compiled_file.write(&opcode, sizeof(char));
               
                current_file_position++;

                if(in_loop)
                    loop_offset[index]++; 
                
                break;
            }

            case '+':
            {
                opcode = inc_mem;
                compiled_file.write(&opcode, sizeof(char));
                opcode = si0;
                compiled_file.write(&opcode, sizeof(char));
                
                current_file_position += 2;

                if(in_loop)
                    loop_offset[index] += 2;              
                
                break;
            }

            case '-':
            {
                opcode = inc_mem;
                compiled_file.write(&opcode, sizeof(char));
                opcode = si0 | 0x8;
                compiled_file.write(&opcode, sizeof(char));
                
                current_file_position += 2;

                if(in_loop)
                    loop_offset[index] += 2;               
                
                break;
            }

            case '.':
            {
                opcode = mov;
                compiled_file.write(&opcode, sizeof(char));
                opcode = si0;
                compiled_file.write(&opcode, sizeof(char));
                opcode = intr;
                compiled_file.write(&opcode, sizeof(char));
                opcode = 0x10;
                compiled_file.write(&opcode, sizeof(char));
                
                current_file_position += 4;

                if(in_loop)
                    loop_offset[index] += 4;
                
                break;

            }

            case '[':
            {
                    
                in_loop = true;
            
                loops.push(current_file_position);

                opcode = test;
                compiled_file.write(&opcode, sizeof(char));
                
                opcode = si0;
                compiled_file.write(&opcode, sizeof(char));

                opcode = 0x0;
                compiled_file.write(&opcode, sizeof(char));
                
                opcode = jz;
                compiled_file.write(&opcode, sizeof(char));

                opcode = 0x10;
                compiled_file.write(&opcode, sizeof(char));
                 
               

                printf("loops status: %d\n", loops.top());
                
                index++;
                break;
            }
            

            case ']':
            {
                in_loop = false;

                opcode = jmp_shrt;
                compiled_file.write(&opcode, sizeof(char));
                
                
                opcode = (loop_offset[index] + 2 + 5) * (-1);
                compiled_file.write(&opcode, sizeof(char));


                compiled_file.seekg(std::ios::beg + loops.top() + 4);
                printf("loop_offset: %d\n", loop_offset[index]);
                loops.pop();

                compiled_file.write((char*)&loop_offset[index], sizeof(char));
                loop_offset[index] = 0x0;
                
                index--;

                break;

            }

            case ' ':
            {
                break;
            }



            case '\n':
            {
                break;
            }

            default:
            {
                printf("unrecognized command. character: %d\n", i);
                exit(1);
            }
        

        }

    }

    printf("character counter: %d\n", i);
    


}


