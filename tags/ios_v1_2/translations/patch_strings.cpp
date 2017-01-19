#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

std::map<std::string, int> strings;

std::map<std::string, int>::const_iterator find_match(const std::string &src, std::string::size_type &pos)
{
    for (std::map<std::string, int>::const_iterator it = strings.begin(); it != strings.end(); ++it) {
        pos = src.find(it->first);

        if (pos != std::string::npos)
            return it;
    }

    return strings.end();
}
void parse_file(const std::string &name)
{
    std::string result;
    if (FILE *f = fopen(name.c_str(), "rb")) {
        while (!feof(f)) {
            char buffer[16384];
            int size = fread(buffer, 1, sizeof(buffer), f);

            if (size > 0)
                result.append(buffer, size);
        }
        fclose(f);
    }
    else {
        std::cerr << "Unable to read from " << name << "\n";
        return;
    }
    std::cerr << "Opened file " << name << " of " << result.size() << " bytes.\n";
    int matches = 0;
    std::string::size_type pos;
    std::map<std::string, int>::const_iterator it;
    while ((it = find_match(result, pos)) != strings.end()) {
        char buffer[20];
        sprintf(buffer, "msg_%d", it->second);
        matches++;
        std::cerr << "Replacing " << it->first << " with " << buffer << '\n';
        result.erase(pos, it->first.size());
        result.insert(pos, buffer);
    }
    if (matches > 0) {
        std::cerr << "Found " << matches << " in " << name << " replacing file...";
        if (FILE *f = fopen(name.c_str(), "w")) {
            fwrite(result.c_str(), 1, result.size(), f);
            fclose(f);
            std::cerr << "OK\n";
        }
        else
            std::cerr << "ERROR!\n";
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [dest dir]\n";
        return 0;
    }
    if (DIR *d = opendir(argv[1])) {
        if (FILE *f = fopen("patch_strings.data", "r")) {
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer) - 1, f)) {               
                int idx = atoi(buffer);
                if (idx == 0) {
                    std::cerr << "Invalid index value\n";
                    return 0;
                }
                if (!fgets(buffer, sizeof(buffer) - 1, f)) {
                    std::cerr << "Unable to find a value for index " << idx << "\n";
                    return 0;
                }
                std::string value = "\"";
                value += buffer;

                if (*value.rbegin() == '\n')
                    value.erase(value.size() - 1, 1);
                value.push_back('"');
                strings[value] = idx;
            }
            std::cerr << "Trying to patch " << strings.size() << " strings.\n";
            while (dirent *ent = readdir(d)) {
                if (ent->d_type != DT_REG || *ent->d_name == '.')
                    continue;

                if (char *c = strrchr(ent->d_name, '.')) {
                    if (strcasecmp(c + 1, "c") && strcmp(c + 1, "cpp") &&
                        strcmp(c + 1, "cxx") && strcmp(c + 1, "cc"))
                        continue;
                    std::ostringstream filename;
                    filename << argv[1] << '/' << ent->d_name;

                    parse_file(filename.str());
                }
            }
            fclose(f);
        }
        else
            std::cerr << "Unable to open patch_strings.data, aborting!\n";
        closedir(d);
    }
    else
        std::cerr << "Unable to open directory " << argv[1] << ", aborting!\n";
}
