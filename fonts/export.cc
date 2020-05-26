#include <fstream>
#include <iostream>
#include <iomanip>
#include <list>

struct Glyph
{
    int code;
    int start;
    int width;
};

struct Font
{
    int width, height;
    int gheight;
    std::list<Glyph> glyphs;
};

static bool parse_font( const char *file_name, Font &font )
{
    std::ifstream input(file_name);
    if (input.good())
    {
        int state = 0;

        while (input.good())
        {
            std::string line;
            std::getline(input, line);

            if (line.empty() || line[0] == '#') continue;

            if (state == 0)
            {
                sscanf(line.c_str(), "%d %d %d", &font.width, &font.height, &font.gheight);
                state = 1;
            }
            else
            if (state == 1)
            {
                Glyph glyph;
                sscanf(line.c_str(), "%d %d %d", &glyph.code, &glyph.start, &glyph.width);
                font.glyphs.push_back(glyph);
            }
        }
        input.close();
        return state == 1;
    }
    return false;
}

int main( int argc, char **argv )
{
    Font font;
    if (parse_font(argv[1], font))
    {
        //std::ofstream output(argv[2]);
        std::ostream &output = std::cout;
        //if (output.good())
        {
            output << "#ifndef FONT_" << argv[3] << "_IMPL\n";
            output << "#ifndef FONT_" << argv[3] << "\n";
            output << "#define FONT_" << argv[3] << "\n";
            output << "extern unsigned char " << argv[3] << "_BITMAP[];\n";
            output << "extern int " << argv[3] << "_BITMAP_SIZE;\n";
            output << "extern int " << argv[3] << "_INFO[];\n";
            output << "extern int " << argv[3] << "_GLYPHS[];\n";
            output << "#define " << argv[3] << "_GLYPH_COUNT " << font.glyphs.size() << "\n";
            output << "#endif // FONT_" << argv[3] << "\n\n";

            output << "#else\n";

            output << "int " << argv[3] << "_GLYPHS[] = \n{\n";
            for (auto it = font.glyphs.begin(); it != font.glyphs.end(); ++it)
                output << "   " << it->code << ", " << it->start << ", " << it->width << ",\n";
            output << "};\n";

            output << "unsigned char " << argv[3] << "_BITMAP[] = {";
            std::ifstream bitmap(argv[2], std::ios_base::binary);
            uint8_t buffer[512];
            std::streamsize total = 0;
            while (bitmap.good())
            {
                bitmap.read( (char*)buffer, sizeof(buffer) );
                total += bitmap.gcount();
                for (std::streamsize i = 0, t = bitmap.gcount(); i < t; ++i)
                {
                    if ((i % 32) == 0) output << "\n   ";
                    output << ((buffer[i] > 128) ? 1 : 0) << ",";
                }
            }
            output << "};\n";
            bitmap.close();

            output << "int " << argv[3] << "_BITMAP_SIZE = " << total << ";\n";

            output << "int " << argv[3] << "_INFO[4] = { ";
            output << font.width << ", " << font.height << ", " << font.gheight;
            output << " };\n";

            output << "#endif // FONT_" << argv[3] << "_IMPL\n";
            //output.close();
            return 0;
        }
    }

    return 1;
}