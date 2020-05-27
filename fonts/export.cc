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

static std::string replaceExtension( const std::string &name, const std::string &ext )
{
    auto pos = name.rfind('.');
    if (pos == std::string::npos) "";
    return name.substr(0, pos) + ext;
}

static void createGlyphs( const std::list<std::string> &codes, bool *merged, int width, std::list<Glyph> &glyphs )
{
    auto it = codes.begin();
    int start = -1;
    for (int i = 0; i < width; ++i)
    {
        if (it == codes.end()) break;

        if (start < 0 && merged[i])
            start = i;
        else
        if (start >= 0 && !merged[i])
        {
            Glyph glyph;
            glyph.code = std::strtol(it->c_str(), nullptr, 10);
            glyph.start = start;
            glyph.width = i - start;
            glyphs.push_back(glyph);
            std::cerr << "Found glyph " << (char) glyph.code << " at " << glyph.start << std::endl;
            ++it;
            start = -1;
        }
    }
}

int main( int argc, char **argv )
{
    if (argc != 5)
    {
        std::cerr << "Usage: export <bitmap> <width> <height> <name>" << std::endl;
        return 1;
    }

    std::string bitmapFile = argv[1];
    std::string codeFile = replaceExtension(argv[1], ".code");
    int width = (int) std::strtol(argv[2], nullptr, 10);
    int height = (int) std::strtol(argv[3], nullptr, 10);
    std::string name = argv[4];

    // load the glyph codes order
    std::list<std::string> codes;
    std::string line;
    std::ifstream code(codeFile.c_str());
    if (!code.good()) return 1;
    while (code.good())
    {
        std::getline(code, line);
        if (!line.empty() && line[0] != '#') codes.push_back(line);
    }
    code.close();

    std::ostream &output = std::cout;
    output << "#ifndef FONT_" << name << "_IMPL\n";
    output << "#ifndef FONT_" << name << "\n";
    output << "#define FONT_" << name << "\n";
    output << "extern unsigned char " << name << "_BITMAP[];\n";
    output << "extern int " << name << "_BITMAP_SIZE;\n";
    output << "extern int " << name << "_INFO[];\n";
    output << "extern int " << name << "_GLYPHS[];\n";
    output << "#define " << name << "_GLYPH_COUNT " << codes.size() << "\n";
    output << "#endif // FONT_" << name << "\n\n";
    output << "#else\n";

    // print the font bitmap
    output << "unsigned char " << name << "_BITMAP[] = {";
    std::ifstream bitmap(bitmapFile, std::ios_base::binary);
    bool *merged = new bool[width]();
    uint8_t *buffer = new uint8_t[width];
    std::streamsize total = 0;
    while (bitmap.good())
    {
        bitmap.read( (char*)buffer, width );
        if (width != bitmap.gcount()) continue;
        total += bitmap.gcount();

        // mark the regions with valid pixels
        for (int i = 0; i < width; ++i) merged[i] |= (buffer[i] < 128);
        // export the pixels with '0' (black) or '1' (white)
        for (int i = 0, t = width; i < t; ++i)
        {
            if ((i % 32) == 0) output << "\n   ";
            output << ((buffer[i] != 0) ? 1 : 0) << ",";
        }
    }
    output << "};\n";
    bitmap.close();

    output << "int " << name << "_BITMAP_SIZE = " << total << ";\n";

    // create the glyph mapping
    std::list<Glyph> glyphs;
    createGlyphs(codes, merged, width, glyphs);

    output << "int " << name << "_GLYPHS[] = \n{\n";
    for (auto it = glyphs.begin(); it != glyphs.end(); ++it)
        output << "   " << it->code << ", " << it->start << ", " << it->width << ",\n";
    output << "};\n";

    output << "int " << name << "_INFO[4] = { ";
    output << width << ", " << height << ", " << height;
    output << " };\n";

    output << "#endif // FONT_" << name << "_IMPL\n";
    return 0;
}