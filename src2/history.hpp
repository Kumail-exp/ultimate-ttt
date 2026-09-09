#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include "Board.hpp"
#include "TT.hpp"

// ------------------------------------------------------------
// Helper: trim whitespace from both ends
// ------------------------------------------------------------
static inline std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ------------------------------------------------------------
// Load a single position from a block that looks like:
//
// 0,0,0,0,256,0,0,0,0;
// 4;
// 2;
//
// (the trailing ';' on each line is optional)
// ------------------------------------------------------------
Board loadPosition(const std::string& data)
{
    Board b;
    std::stringstream input(data);
    std::string line;

    // ---------- small[9] ----------
    if (!std::getline(input, line))
        throw std::runtime_error("loadPosition: missing small line");

    line = trim(line);
    if (!line.empty() && line.back() == ';')
        line.pop_back();

    std::stringstream board(line);
    std::string value;
    for (int i = 0; i < 9; ++i)
    {
        if (!std::getline(board, value, ','))
            throw std::runtime_error("loadPosition: expected 9 small values");

        value = trim(value);
        if (value.empty())
            throw std::runtime_error("loadPosition: empty small value");

        b.small[i] = static_cast<uint32_t>(std::stoul(value));
    }

    // ---------- next ----------
    if (!std::getline(input, line))
        throw std::runtime_error("loadPosition: missing next line");

    line = trim(line);
    if (!line.empty() && line.back() == ';')
        line.pop_back();

    b.next = static_cast<uint8_t>(std::stoi(line));

    // ---------- player ----------
    if (!std::getline(input, line))
        throw std::runtime_error("loadPosition: missing player line");

    line = trim(line);
    if (!line.empty() && line.back() == ';')
        line.pop_back();

    b.player = static_cast<uint8_t>(std::stoi(line));

    // ---------- recompute meta ----------
    b.meta = 0;
    for (int i = 0; i < 9; ++i)
    {
        int w = b.checkSmallWin(i);
        if (w)
            b.meta |= (w << (2 * i));
    }

    // ---------- recompute winner ----------
    b.winner = b.checkMetaWin();
    if (b.winner == 0)
    {
        bool full = true;
        for (int i = 0; i < 9; ++i)
        {
            if (((b.meta >> (2 * i)) & 3) == 0 && !b.isSmallFull(i))
            {
                full = false;
                break;
            }
        }
        if (full)
            b.winner = 3;
    }

    // ---------- rebuild Zobrist hash ----------
    // (important for TT / search)
    b.i_hash = Hash(b);          // make sure Hash is declared / visible

    return b;
}

// ------------------------------------------------------------
// Load many positions separated by "==========;"
// ------------------------------------------------------------
std::vector<Board> loadPositions(const std::string& data)
{
    std::vector<Board> boards;
    const std::string separator = "==========;";

    size_t start = 0;
    while (true)
    {
        size_t pos = data.find(separator, start);
        if (pos == std::string::npos)
            break;

        start = pos + separator.size();
        size_t next = data.find(separator, start);

        std::string block = data.substr(
            start,
            next == std::string::npos ? std::string::npos : next - start
        );

        // strip leading whitespace / newlines
        size_t first = block.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            if (next == std::string::npos) break;
            start = next;
            continue;
        }
        block.erase(0, first);

        // also strip trailing whitespace
        size_t last = block.find_last_not_of(" \t\r\n");
        if (last != std::string::npos)
            block.erase(last + 1);

        if (!block.empty())
            boards.push_back(loadPosition(block));

        if (next == std::string::npos)
            break;
        start = next;
    }

    return boards;
}