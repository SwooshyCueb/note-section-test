#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include <elf.h>
#include <sysexits.h>

int main(int _argc, char *_argv[]){
	if (_argc != 2) {
		std::cerr << "please supply path to shared library as the only argument" << std::endl;
		return EX_USAGE;
	}

	std::ifstream lib(_argv[1], std::ios::binary);
	if (!lib) {
		std::cerr << "could not open " << _argv[1] << std::endl;
		return EX_NOINPUT;
	}
	
	const std::array<char, 4> elf_mag{{ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3}};
	std::array<char, 4> elf_mag_in{};
	lib.read(reinterpret_cast<char*>(elf_mag_in.data()), elf_mag_in.size());
	if (elf_mag != elf_mag_in) {
		std::cerr << "ELF magic number does not match" << std::endl;
		return EX_DATAERR;
	}

	std::uint8_t ei_class{};
	lib.read(reinterpret_cast<char*>(&ei_class), sizeof(ei_class));

	lib.seekg(0);

	std::uint64_t e_shoff{};
	std::uint16_t e_shentsize{};
	std::uint64_t e_shnum{};
	std::uint32_t e_shstrndx{};
	if (ei_class == ELFCLASS32) {
		Elf32_Ehdr elf_hdr_in{};
		lib.read(reinterpret_cast<char*>(&elf_hdr_in), sizeof(elf_hdr_in));
		e_shoff = elf_hdr_in.e_shoff;
		e_shentsize = elf_hdr_in.e_shentsize;
		e_shnum = elf_hdr_in.e_shnum;
		e_shstrndx = elf_hdr_in.e_shstrndx;
	}
	else if (ei_class == ELFCLASS64) {
		Elf64_Ehdr elf_hdr_in{};
		lib.read(reinterpret_cast<char*>(&elf_hdr_in), sizeof(elf_hdr_in));
		e_shoff = elf_hdr_in.e_shoff;
		e_shentsize = elf_hdr_in.e_shentsize;
		e_shnum = elf_hdr_in.e_shnum;
		e_shstrndx = elf_hdr_in.e_shstrndx;
	}
	else {
		std::cerr << "Unknown or invalid ELFCLASS 0x" << std::hex << ei_class << std::endl;
		return EX_DATAERR;
	}

	if (e_shoff == 0) {
		std::cerr << "ELF does not contain section table" << std::endl;
	}

	// seek to section table
	lib.seekg(e_shoff);

	// some data may be contained in the first entry
	if ((e_shnum == 0) || (e_shstrndx == SHN_XINDEX)) {
		if (ei_class == ELFCLASS32) {
			Elf32_Shdr s_hdr_in{};
			lib.read(reinterpret_cast<char*>(&s_hdr_in), sizeof(s_hdr_in));
			if (e_shnum == 0) {
				e_shnum = s_hdr_in.sh_size;
			}
			if (e_shstrndx == SHN_XINDEX) {
				e_shstrndx = s_hdr_in.sh_link;
			}
		}
		else {
			Elf64_Shdr s_hdr_in{};
			lib.read(reinterpret_cast<char*>(&s_hdr_in), sizeof(s_hdr_in));
			if (e_shnum == 0) {
				e_shnum = s_hdr_in.sh_size;
			}
			if (e_shstrndx == SHN_XINDEX) {
				e_shstrndx = s_hdr_in.sh_link;
			}
		}
	}

	// offset of string table section header
	const std::uint64_t e_shstroff = e_shoff + (e_shstrndx * e_shentsize);

	// seek to string table section header
	lib.seekg(e_shstroff);

	std::uint64_t e_strtoff{};
	std::uint64_t e_strtsz{};
	if (ei_class == ELFCLASS32) {
		Elf32_Shdr s_hdr_in{};
		lib.read(reinterpret_cast<char*>(&s_hdr_in), sizeof(s_hdr_in));
		e_strtoff = s_hdr_in.sh_offset;
		e_strtsz = s_hdr_in.sh_size;
	}
	else {
		Elf64_Shdr s_hdr_in{};
		lib.read(reinterpret_cast<char*>(&s_hdr_in), sizeof(s_hdr_in));
		e_strtoff = s_hdr_in.sh_offset;
		e_strtsz = s_hdr_in.sh_size;
	}

	// seek to section name string table
	lib.seekg(e_strtoff);

	std::vector<char> e_strtbl_vec(e_strtsz);
	lib.read(e_strtbl_vec.data(), e_strtsz);

	for (std::uint64_t sh_idx = 0; sh_idx < e_shnum; sh_idx++) {
		// seek to section header
		lib.seekg(e_shoff + (sh_idx * e_shentsize));
		std::uint32_t str_off{};
		lib.read(reinterpret_cast<char*>(&str_off), sizeof(str_off));
		std::cout << &(e_strtbl_vec.data()[str_off]) << std::endl;
	}

	return 0;
}
