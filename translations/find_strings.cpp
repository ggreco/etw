#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <set>

std::set<std::string> strings;

bool need_translation(FILE *f)
{
    if (feof(f))
        return true;

    char buffer[8];
    fseek(f, -1, SEEK_CUR);
    if (fread(buffer, 1, 4, f) != 4) 
        return true;
//    std::cerr << "Buff: <" << std::string(buffer, 4) << "> ";
    fseek(f, -4, SEEK_CUR);
    return strncmp(buffer, "/*-*", 4) != 0;
}

void find_strings(const char *name)
{
    if (FILE *f = fopen(name, "rb")) {
        static const char *comment_end = "*/", *comment_start = "/*";
        bool found = false;
        bool is_comment = false;
        bool is_cpp_comment = false;
        int line = 1, comment_idx = 0;
        std::string latest, line_buf;
        while (!feof(f)) {
            int c = fgetc(f);

            if (c == '\n') {
                line_buf.clear();
                line++;
                is_cpp_comment = false;
                continue;
            }
            else if (is_cpp_comment) {
            }
            else if (is_comment) {
                if (c == comment_end[comment_idx]) {
                    comment_idx++;
                    if (comment_idx == 2) {
//                        std::cerr << "Found c comment end at line " << line << '\n';
                        is_comment = false;
                        comment_idx = 0;
                    }
                }
            }
            else if (c == '"') {
                if (!found) {
                    latest.clear();
                    found = true;
                }
                else {
                    while (!feof(f)) {
                        int c2 = fgetc(f);
                        if (c2 == '\n') {
                            line++;
                        }
                        else if (!isspace(c2))
                            break;
                    }
                    
                    if (line_buf.find("D(bug") == std::string::npos &&
                        line_buf.find("#include") == std::string::npos &&
                        line_buf.find("extern ") == std::string::npos &&
                        need_translation(f)) {
                        strings.insert(latest);
                        std::cerr << name << ":" << line << " <" << latest << ">\n";
                    }
                    found = false;
                }
            }
            else if (found)
                latest.push_back((char)c);
            else if (c == comment_start[comment_idx]) {
                comment_idx++;

                if (comment_idx == 2) {
                    is_comment = true;
                    comment_idx = 0;
//                    std::cerr << "Found comment start at line " << line << '\n';
                }
            }
            else if (!is_comment && comment_idx == 1 && c == '/') {
                is_cpp_comment = true;
//                std::cerr << "Found CPP comment start at line " << line << '\n';
            }
            else
                comment_idx = 0;
            
            line_buf.push_back((char)c);
        }
        fclose(f);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [start index] [files]\n";
        return 0;
    }
    int index = atoi(argv[1]);
    for (int i = 2; i < argc; ++i) 
        find_strings(argv[i]);

    std::cerr << "\nFound a total of " << strings.size() << " unique strings to be translated\n\n";

    FILE *hdr = fopen("strings_delta.h", "w"),
         *csrc = fopen("strings_delta.c", "w"),
         *ctg = fopen("strings_delta.cd", "w"),
         *pch = fopen("patch_strings.data", "w");

    if (!hdr || !csrc || !ctg || !pch) {
        std::cerr << "Unable to open destination files!\n!";
        return -1;
    }

    for (std::set<std::string>::iterator it = strings.begin(); it != strings.end(); ++it, ++index) {
        fprintf(hdr, "extern struct __LString __LTool__%d;\n"
                     "#define msg_%d __LTool__%d.Str\n", index, index, index);

        fprintf(csrc, "struct {void *Prev;int Cat_ID;char Str[(%d)+1];} __LTool__%d = {&__LTool__%d,%d,\"%s\"};\n", 
                it->size() * 2 + 15, index, index - 1, index, it->c_str());
        fprintf(ctg, "msg_%d (%d//%d)\n"
                     "%s\n;\n", index, index, it->size() * 2 + 15, it->c_str());

        fprintf(pch, "%d\n%s\n", index, it->c_str());
    }
    fclose(ctg);
    fclose(csrc);
    fclose(hdr);
}
