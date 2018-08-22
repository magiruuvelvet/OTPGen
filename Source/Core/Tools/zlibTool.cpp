#include "zlibTool.hpp"

#include <zlib.h>

const std::string zlibTool::uncompress(const std::string &data, bool *success)
{
    if (data.size() <= 4)
    {
        if (success) *success = false;
        return std::string();
    }

    std::string result;

    int ret;
    z_stream strm;
    static const int CHUNK_SIZE = 1024;
    char out[CHUNK_SIZE];

    /* allocate inflate state */
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    strm.avail_in = static_cast<uInt>(data.size());
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));

    // gzip decoding
    ret = inflateInit2(&strm, 15 + 32);
    if (ret != Z_OK)
    {
        if (success) *success = false;
        return std::string();
    }

    // run inflate()
    do {
        strm.avail_out = CHUNK_SIZE;
        strm.next_out = (Bytef*)(out);

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR)
        {
            // state not clobbered
            if (success) *success = false;
            return std::string();
        }

        switch (ret)
        {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR; // and fall through
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void) inflateEnd(&strm);
                if (success) *success = false;
                return std::string();
        }

        result.append(out, CHUNK_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    // clean up and return
    inflateEnd(&strm);
    if (success) *success = true;
    return result;
}

const std::string zlibTool::uncompress(const char *data, int size, bool *success)
{
    return uncompress(std::string(data, data + size), success);
}
