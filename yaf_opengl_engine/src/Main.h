#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "YafScene.h"

#define NTHEMES 3
static const char* THEMES[NTHEMES] = { "cequis_default.xml", "cequis_pokemon.xml", "cequis_pokemon_2.xml" };

YafScene* ParseYafFile(const std::string& file);

#endif
