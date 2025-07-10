#ifndef __RAM_HPP
#define __RAM_HPP

#include <cstddef>
#include <vector>
#include <tuple>
#include <algorithm>
#include <format>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <spdlog/spdlog.h>
#include <cstring>

#include <iostream>
template<class TupType, size_t... I>
void print(const TupType& _tup, std::index_sequence<I...>)
{
    std::cout << "(";
    (..., (std::cout << (I == 0? "" : ", ") << std::get<I>(_tup)));
    std::cout << ")\n";
}

template<class... T>
void print (const std::tuple<T...>& _tup)
{
    print(_tup, std::make_index_sequence<sizeof...(T)>());
}

template <typename A, typename D>
class MmapEntry {
public:
    A address;
    std::size_t size;
    D *buf;
    bool writable;
    const char *id;
};

template <typename A, typename D>
class RAM {
public:
    RAM() {
        memmap = std::vector<memmapEntry>();
    }
    // ~RAM(); // TODO

    // For reading only
    D operator[](A addr) {
        return read(addr);
    }

    D *ptr(A addr) {
        auto iter = std::find_if(memmap.rbegin(), memmap.rend(), [&addr](const memmapEntry &x) {
            A addr_begin = std::get<0>(x);
            A addr_end = addr_begin + std::get<1>(x) - 1;
            // spdlog::debug(std::format("{:04x}:{:04x}", addr_begin, addr_end));
            return (addr_begin <= addr) && (addr <= addr_end);
        });

        if(iter == memmap.rend()) {
            spdlog::warn(std::format("Reading from unmapped address 0x{:x}", addr));
            return 0;
        }

        memmapEntry region = *iter;
        A offset = addr - std::get<0>(region);
        return std::get<2>(region) + offset;
    }

    D read(A addr) {
        auto iter = std::find_if(memmap.rbegin(), memmap.rend(), [&addr](const memmapEntry &x) {
            A addr_begin = std::get<0>(x);
            A addr_end = addr_begin + std::get<1>(x) - 1;
            // spdlog::debug(std::format("{:04x}:{:04x}", addr_begin, addr_end));
            return (addr_begin <= addr) && (addr <= addr_end);
        });

        if(iter == memmap.rend()) {
            spdlog::warn(std::format("Reading from unmapped address 0x{:x}", addr));
            return 0;
        }

        memmapEntry region = *iter;
        A offset = addr - std::get<0>(region);
        return std::get<2>(region)[offset];
    }
    void write(A addr, D data) {
        // spdlog::debug("Writing {:02x} to {:04x}", data, addr);
        auto iter = std::find_if(memmap.rbegin(), memmap.rend(), [&addr](const memmapEntry &x) {
            A addr_begin = std::get<0>(x);
            A addr_end = addr_begin + std::get<1>(x);
            return (addr_begin <= addr) && (addr < addr_end);
        });

        if(iter == memmap.rend()) {
            spdlog::warn(std::format("Writing to unmapped address 0x{:x}", addr));
            return;
        }

        memmapEntry region = *iter;
        A offset = addr - std::get<0>(region);
        bool writable = std::get<3>(region);
        if(writable) {
            std::get<2>(region)[offset] = data;
        } else {
            spdlog::warn(std::format("Writes to read-only memory ignored"));
        }
    }

    // TODO: Check for past end of addressable memory
    void mapMem(const char *id, A addr, std::size_t n, bool writable = false) {
        D *buf = (D *)calloc(n, sizeof(D));
        memmapEntry entry = memmapEntry(addr, n, buf, writable, id);
        memmap.push_back(entry);
    }

    void mapBuf(const char *id, A addr, std::size_t n, D *buf, bool writable = false) {
        memmapEntry entry = memmapEntry(addr, n, buf, writable, id);
        memmap.push_back(entry);
    }

    // Parameter n is ignored on read-only mapping (uses size of file).
    void mapFil(const char *id, A addr, std::size_t n, const char *filepath, bool writable = false) {
        D *buf;

        if(writable) {
            int file = open(filepath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
            posix_fallocate(file, 0, n * sizeof(D));
            buf = (D *)mmap(NULL, n * sizeof(D), PROT_WRITE, MAP_SHARED, file, 0);
        } else {
            // This copies file contents, which means it cannot write back to file
            std::ifstream file(filepath, std::ios::binary | std::ios::ate);
            if(!file.is_open()) {
                spdlog::error(std::format("Failed to open \"{}\"", filepath));
                return;
            } else {
                n = file.tellg();
                buf = (D *)calloc(n, sizeof(D));
                file.seekg(0, std::ios::beg);
                file.read((char *)buf, n);
            }
        }

        memmapEntry entry = memmapEntry(addr, n, buf, writable, id);
        memmap.push_back(entry);
    }

    void unmap(const char *id) {
        auto iter = std::find_if(memmap.rbegin(), memmap.rend(), [&id](const memmapEntry &x) {
            const char *idf = std::get<4>(x);
            return !strncmp(id, idf, 16);
        });

        if(iter == memmap.rend()) {
            spdlog::warn(std::format("Could not find mapping with ID: {}", id));
            return;
        }

            A addr = std::get<0>(*iter);
            std::size_t s = std::get<1>(*iter);
            bool wr = std::get<3>(*iter);
            const char *idx = std::get<4>(*iter);

            spdlog::debug("E: {:04x} {:04x}: {} ({})", addr, s, wr ? "RW" : "RO", idx);
        memmap.erase(--(iter.base()));
    }

    void printMap() {
        for(auto it = memmap.begin(); it != memmap.end(); it++) {
            A addr = std::get<0>(*it);
            std::size_t s = std::get<1>(*it);
            bool wr = std::get<3>(*it);
            const char *id = std::get<4>(*it);

            // spdlog::debug("{:04x} {:04x}: {} ({})", addr, s, wr ? "RW" : "RO", id);
            memmapEntry entry = *it;
            // spdlog::debug(std::format("{}", entry));
        }
    }

private:
    typedef std::tuple<A, std::size_t, D *, bool, const char *> memmapEntry;
    std::vector<memmapEntry> memmap;
};

#endif






