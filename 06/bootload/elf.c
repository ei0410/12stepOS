#include "defines.h"
#include "elf.h"
#include "lib.h"

//elf header
struct elf_header {
	struct {
		unsigned char magic[4];    //magic number
		unsigned char class;       //32bit or 64bit
		unsigned char format;      //endian information
		unsigned char version;     //elf format version
		unsigned char abi;         //a kind of OS
		unsigned char abi_version; //OS version
		unsigned char reserve[7];  //unknown
	} id;
	short type;                    //file type
	short arch;                    //a kind of CPU
	long version;                  //elf version
	long entry_point;              //address for start
	long program_header_offset;    //position of program header table
	long section_header_offset;    //position of section header table
	long flags;
	short header_size;             
	short program_header_size;
	short program_header_num;
	short section_header_size;
	short section_header_num;
	short section_name_index;
};

//program header
struct elf_program_header {
	long type;          //a kind of segment
	long offset;        //position in file
	long virtual_addr;  //VA
	long physical_addr; //PA
	long file_size;     
	long memory_size;
	long flags;
	long align;
};

//check of elf header
static int elf_check(struct elf_header *header)
{
	if (memcmp(header->id.magic, "\x7f" "ELF", 4))
		return -1;

	if (header->id.class   != 1) return -1; //ELF32
	if (header->id.format  != 2) return -1; //Big endian
	if (header->id.version != 1) return -1; //version 1
	if (header->type       != 2) return -1; //exe file
	if (header->version    != 1) return -1; //version 1

	//H8/300 or H8/300H
	if ((header->arch != 46) && (header->arch != 47)) return -1;

	return 0;
}

// load per segment
static int elf_load_program(struct elf_header *header)
{
	int i;
	struct elf_program_header *phdr;

	for (i = 0; i < header->program_header_num; i++) { //loop per segment
		//get program header
		phdr = (struct elf_program_header *)
			((char *)header + header->program_header_offset + 
			header->program_header_size * i);

		if (phdr->type != 1) //check whether enable load or not
			continue;

		memcpy((char *)phdr->physical_addr, (char *)header + phdr->offset, 
			phdr->file_size);
		memset((char *)phdr->physical_addr + phdr->file_size, 0, 
			phdr->memory_size - phdr->file_size);
	}

	return 0;
}

char *elf_load(char *buf)
{
	struct elf_header *header = (struct elf_header *)buf;

	if (elf_check(header) < 0)
		return NULL;

	if (elf_load_program(header) < 0)
		return NULL;

	return (char *)header->entry_point;
}
