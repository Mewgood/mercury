#include "d2cdkey.hpp"

#include "utility.hpp"
#include "bsha1.hpp"

#include "string.hpp"

#include <iostream>

//code largely copied from JBLS

namespace
{
        ulong const alpha_map[] =
        {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0x00, 0xFF, 0x01, 0xFF, 0x02, 0x03,
                0x04, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                0x0C, 0xFF, 0x0D, 0x0E, 0xFF, 0x0F, 0x10, 0xFF,
                0x11, 0xFF,     0x12, 0xFF, 0x13, 0xFF, 0x14, 0x15,
                0x16, 0xFF, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                0x0C, 0xFF, 0x0D, 0x0E, 0xFF, 0x0F, 0x10, 0xFF,
                0x11, 0xFF, 0x12, 0xFF, 0x13, 0xFF, 0x14, 0x15,
                0x16, 0xFF, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF,     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
}

char get_hex_digit(ulong byte)
{
        byte &= 0xF;
        if(byte < 10)
                return static_cast<char>(byte + 0x30);
        else
                return static_cast<char>(byte + 0x37);
}

ulong convert_hex_digit(char input)
{
        if(input >= '0' && input <= '9')
                return static_cast<ulong>(input - 0x30);
        else
                return static_cast<ulong>(input - 0x37);
}

std::string read_hex_string_value(std::string const & input)
{
        return dword_to_string(nil::string::string_to_number<ulong>(input, std::ios_base::hex));
}

bool hash_d2key(std::string const & cdkey, ulong client_token, ulong server_token, std::string & output, std::string & public_value)
{
        ulong checksum = 0;
        ulong n, n2, v, v2;
        char c1, c2, c;

        std::string manipulated_key = cdkey;

        for(std::size_t i = 0; i < cdkey.length(); i += 2)
        {
                c1 = static_cast<char>(alpha_map[(signed char)cdkey[i]]);
                n = c1 * 3;
                c2 = static_cast<char>(alpha_map[(signed char)cdkey[i + 1]]);
                n = static_cast<ulong>(c2) + 8 * n;

                if (n >= 0x100)
                {
                        n -= 0x100;
                        checksum |= 1 << (i >> 1);
                }
                n2 = n;
                n2 >>= 4;
                manipulated_key[i] = get_hex_digit(n2);
                manipulated_key[i + 1] = get_hex_digit(n);
        }

        v = 3;
        for (int i = 0; i < 16; i++)
        {
                n = convert_hex_digit(manipulated_key[i]);
                n2 = v * 2;
                n ^= n2;
                v += n;
        }

        v &= 0xFF;
        if(v != checksum)
                //invalid CD key
                return false;

        for (int i = 15; i >= 0; i--)
        {
                c = manipulated_key[i];
                if (i > 8)
                        n = i - 9;
                else
                        n = 0xF - (8 - i);
                n &= 0xF;
                c2 = manipulated_key[n];
                manipulated_key[i] = c2;
                manipulated_key[n] = c;
        }

        v2 = 0x13AC9741;

        for(long i = 15; i >= 0; i--)
        {
                c = nil::string::to_upper(manipulated_key[i]);
                manipulated_key[i] = c;
                if (c <= '7')
                {
                        v = v2;
                        c2 = static_cast<char>(v & 0xFF);
                        c2 &= 7;
                        c2 ^= c;
                        v >>= 3;
                        manipulated_key[i] = c2;
                        v2 = v;
                }
                else if (c < 'A')
                {
                        c2 = static_cast<char>(i);
                        c2 &= 1;
                        c2 ^= c;
                        manipulated_key[i] = c2;
                }
        }

        public_value = read_hex_string_value(manipulated_key.substr(2, 6));

        std::string hash_data = dword_to_string(client_token);
        hash_data += dword_to_string(server_token);
        hash_data += read_hex_string_value(manipulated_key.substr(0, 2));
        hash_data += public_value;
        hash_data += dword_to_string(0);
        hash_data += read_hex_string_value(manipulated_key.substr(8, 8));

        output = bsha1(hash_data);
        return true;
}
